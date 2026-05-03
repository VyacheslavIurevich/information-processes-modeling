import subprocess
import os

# --- НАСТРОЙКИ ---
PROJECT_NAME = "server"
# Путь к исполняемому файлу (может отличаться в зависимости от ОС)
EXECUTABLE = "./src/server" if os.name != 'nt' else "./src/server.exe"
INI_FILE = "omnetpp.ini"
MAX_STAFF_TO_TRY = 5
TARGET_THRESHOLD = 2.0

def run_simulation(staff_count):
    print(f"--> Запуск симуляции: программистов = {staff_count}...", end=" ", flush=True)
    
    # Запуск OMNeT++ через CLI
    # -u Cmdenv: консольный интерфейс (без графики)
    # -c General: имя конфигурации в ini
    # --*.repairCenter.numProgrammers: переопределение параметра прямо из командной строки
    cmd = [
        EXECUTABLE,
        "-u", "Cmdenv",
        "-c", "General",
        f"--*.repairCenter.numProgrammers={staff_count}",
        "--result-dir=results_cli" # сохраняем отдельно
    ]
    subprocess.run(cmd)
    print("Готово.")

def get_result(staff_count):
    # Используем scavetool для экспорта нужного скаляра в CSV формат прямо в память
    # Ищем результат failedServers:timeavg
    sca_file = f"results_cli/General-*.sca" # OMNeT++ добавит индекс, используем маску
    
    # Команда извлечения: s query -f "name(failedServers:timeavg)" -O csv
    cmd = [
        "opp_scavetool", "export", 
        "-o", "-", # вывод в stdout
        "-F", "CSV-R", # формат CSV
        "results_cli/*.sca"
    ]
    
    res = subprocess.run(cmd, capture_output=True, text=True)
    
    # Парсим CSV (ищем строку со значением)
    for line in res.stdout.splitlines():
        if "failedServers:timeavg" in line:
            # В CSV-R формате значение обычно в последней колонке
            parts = line.split(",")
            return float(parts[-1].strip('"'))
    return None

def main():
    print(f"Поиск минимального кол-ва персонала (цель < {TARGET_THRESHOLD})...")
    print("-" * 50)
    
    best_n = None
    
    for n in range(1, MAX_STAFF_TO_TRY + 1):
        # Очистим старые результаты перед запуском
        if os.path.exists("results_cli"):
            for f in os.listdir("results_cli"): os.remove(os.path.join("results_cli", f))
        else:
            os.makedirs("results_cli")

        run_simulation(n)
        avg_failed = get_result(n)
        
        if avg_failed is not None:
            print(f"    Среднее число неисправных серверов: {avg_failed:.4f}")
            if avg_failed <= TARGET_THRESHOLD:
                best_n = n
                break
        else:
            print("    Ошибка: не удалось получить данные из симуляции.")

    print("-" * 50)
    if best_n:
        print(f"ОТВЕТ: Минимальное количество программистов = {best_n}")
    else:
        print("ОТВЕТ: Решение не найдено в заданном диапазоне.")

if __name__ == "__main__":
    main()
