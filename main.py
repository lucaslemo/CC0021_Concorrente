import time
from math import floor, sqrt


def do_list(value, list01):
    for i in range(2, value + 1):
        list01.append(i)


def found_on_list(value, list01):
    max_value = floor(sqrt(value))
    for i in list01:
        if i > max_value:
            break
        else:
            for j in list01:
                if j > i:
                    if j % i == 0:
                        list01.remove(j)


def main():
    value_limit = int(input("Insert limit value: "))
    start = time.time()
    prime_list = list()
    do_list(value_limit, prime_list)
    end = time.time()
    print("Time needed for do the complete list: {}s".format(end - start))
    start = time.time()
    found_on_list(value_limit, prime_list)
    end = time.time()
    print("Time needed for do the prime list: {}s".format(end - start))
    print("The length of list: {}\nList:\n{}\n".format(len(prime_list), prime_list))


if __name__ == "__main__":
    main()
