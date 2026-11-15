import quecgnss


def main():
    ret = quecgnss.init()
    if ret == 0:
        print('GNSS init ok.')
    else:
        print('GNSS init failed.')
        return -1
    data = quecgnss.read(4096)
    print("Code : ")
    print(data)
    print("\n")
    print(data[1].decode())

    quecgnss.gnssEnable(0)


if __name__ == '__main__':
    main()