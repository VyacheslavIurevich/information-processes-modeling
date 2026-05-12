import os
import subprocess

# -------------------------------------------------------------------------
# GLOBAL CONFIGURATION
# -------------------------------------------------------------------------
PROJECT_NAME = "server"

# Determine the binary path based on the operating system.
# 'nt' refers to Windows (.exe), otherwise we assume a Linux/macOS binary.
EXECUTABLE = "./src/server" if os.name != "nt" else "./src/server.exe"

INI_FILE = "omnetpp.ini"

# Search constraints
MAX_STAFF_TO_TRY = 5
TARGET_THRESHOLD = 2.0  # Our goal: Average failed servers must be < 2.0


def run_simulation(staff_count):
    """
    Executes the OMNeT++ model as a background process.
    """
    print(
        f"--> Running simulation: staff_count = {staff_count}...", end=" ", flush=True
    )

    # Building the CLI command:
    # -u Cmdenv: Runs without GUI
    # -c General: Uses the [General] section from omnetpp.ini
    # --parameter=value: It overrides the .ned/.ini
    # values directly from the command line without modifying files.
    cmd = [
        EXECUTABLE,
        "-u",
        "Cmdenv",
        "-c",
        "General",
        f"--*.repairCenter.numProgrammers={staff_count}",
        "--result-dir=results_cli",
    ]

    # Execute and wait for completion
    subprocess.run(cmd, capture_output=True)
    print("Done.")


def get_result():
    """
    Extracts the 'timeavg' statistic from the generated .sca file.
    """
    # We use opp_scavetool here to convert binary result files into a readable CSV format
    # on the fly and pipe it directly into Python
    cmd = [
        "opp_scavetool",
        "export",
        "-o",
        "-",  # Stream output to stdout instead of a file
        "-F",
        "CSV-R",  # Use Raw CSV format for easier parsing
        "results_cli/*.sca",  # Process all scalar files in the results dir
    ]

    res = subprocess.run(cmd, capture_output=True, text=True)

    # Parse the CSV output line by line
    for line in res.stdout.splitlines():
        # We are looking specifically for the time-weighted average signal
        if "failedServers:timeavg" in line:
            # In CSV-R format, the numeric value is in the last column
            parts = line.split(",")
            return float(parts[-1].strip('"'))

    return None


def main():
    """
    Optimization loop: Starts from 1 programmer and increments until the
    performance target is met.
    """
    print(f"Searching for minimum staff (Target: < {TARGET_THRESHOLD})...")
    print("-" * 50)

    best_n = None

    for n in range(1, MAX_STAFF_TO_TRY + 1):
        # CLEANUP: Remove old results before each run
        if os.path.exists("results_cli"):
            for f in os.listdir("results_cli"):
                os.remove(os.path.join("results_cli", f))
        else:
            os.makedirs("results_cli")

        # 1. Run the simulation for the current number of programmers (n)
        run_simulation(n)

        # 2. Extract the avg_failed metric
        avg_failed = get_result()

        if avg_failed is not None:
            print(f"    Average failed servers: {avg_failed:.4f}")

            # 3. Check if we met the requirement
            if avg_failed <= TARGET_THRESHOLD:
                best_n = n
                break  # Success! No need to test higher staff counts
        else:
            print("    Error: Could not retrieve simulation data.")

    print("-" * 50)
    if best_n:
        print(f"RESULT: Minimum programmers required = {best_n}")
    else:
        print("RESULT: No solution found within the given range.")


if __name__ == "__main__":
    main()
