import threading
import time
from math import floor, sqrt


def do_list(value, list01):
    start = time.time()
    for i in range(2, value + 1):
        list01.append(i)
    end = time.time()
    return end - start


def func_thread(key_list, list01, index01, index02):
    print("{} it started...".format(threading.current_thread().getName()))
    for i in key_list:
        for j in range(index01, index02):
            if list01[j] % i == 0:
                list01[j] = 0
    print("{} Finish.".format(threading.current_thread().getName()))


def crivo(value, list01):
    for i in range(value - 1):
        if list01[i] == 0:
            continue
        else:
            for j in range(i + 1, len(list01)):
                if list01[j] == 0:
                    continue
                elif list01[j] % list01[i] == 0:
                    list01[j] = 0


def serial():
    # Inicializa variáveis
    non_valid = True
    value_limit = int
    full_list = list()
    prime_list = list()

    # Valida entrada
    while non_valid:
        value_limit = int(input("Insert positive limit value: "))
        if value_limit > 0:
            non_valid = False
        else:
            print("Invalid!")

    # Encontra o valor chave (Raiz da quantidade total)
    key_value = floor(sqrt(value_limit))

    # Cria lista completa de 2 até valor limite
    time_full = do_list(value_limit, full_list)

    # Chama função crivo
    print("Iniciando Crivo...")
    start = time.time()
    crivo(key_value, full_list)

    # Copia os primos para a lista final
    print("Copiando para lista final...")
    for i in full_list:
        if i != 0:
            prime_list.append(i)
    end = time.time()

    # Salva as informações em um arquivo .txt
    print("Salvando os resultados.")
    file = open("Result_for_{}_Serial_Algorithm.txt".format(value_limit), "w")
    print("Used Serial algorithm for max value: {}".format(value_limit), file=file)
    print("Time needed for do the complete list: {}s".format(time_full), file=file)
    print("Time needed for do the prime list: {}s".format(end - start), file=file)
    print("The length of list: {}\nList:\n{}\n".format(len(prime_list), prime_list), file=file)


def thread():
    # Inicializa variáveis
    non_valid = True
    value_limit = int
    full_list = list()
    prime_list = list()

    # Valida entrada
    while non_valid:
        value_limit = int(input("Insert positive limit value: "))
        if value_limit > 0:
            non_valid = False
        else:
            print("Invalid!")

    # Encontra o valor chave (Raiz da quantidade total)
    key_value = floor(sqrt(value_limit))

    # Cria lista completa de 2 até valor limite
    time_full = do_list(value_limit, full_list)

    # Cria lista primos chaves
    key_list = full_list[:key_value - 1]
    crivo(key_value, key_list)
    for i in key_list:
        if i != 0:
            prime_list.append(i)
    key_list.clear()

    # Tamanho da partição da lista
    div = int(len(full_list[key_value - 1:]) / 4)
    index0 = key_value - 1

    start = time.time()
    t1 = threading.Thread(target=func_thread, args=(prime_list, full_list, index0, index0 + div), name="T1")
    t2 = threading.Thread(target=func_thread, args=(prime_list, full_list, index0 + div, index0 + div*2), name="T2")
    t3 = threading.Thread(target=func_thread, args=(prime_list, full_list, index0 + div*2, index0 + div*3), name="T3")
    t4 = threading.Thread(target=func_thread, args=(prime_list, full_list, index0 + div*3, value_limit - 1), name="T4")
    t1.start()
    t2.start()
    t3.start()
    t4.start()

    t1.join()
    t2.join()
    t3.join()
    t4.join()

    # Copia os primos para a lista final
    print("Copiando para lista final...")
    for i in full_list[key_value - 1:]:
        if i != 0:
            prime_list.append(i)
    end = time.time()

    # Salva as informações em um arquivo .txt
    print("Salvando os resultados.")
    file = open("Result_for_{}_Thread_Algorithm.txt".format(value_limit), "w")
    print("Used Thread algorithm for max value: {}".format(value_limit), file=file)
    print("Time needed for do the complete list: {}s".format(time_full), file=file)
    print("Time needed for do the prime list: {}s".format(end - start), file=file)
    print("The length of list: {}\nList:\n{}\n".format(len(prime_list), prime_list), file=file)


def main():
    switch = 0
    while switch == 0:
        switch = int(input("Select:\n(1) for Serial process\n(2) for thread process\n->"))
        if switch == 1:
            serial()
        elif switch == 2:
            thread()
        else:
            switch = 0
            print("Invalid!\n")


if __name__ == "__main__":
    main()
