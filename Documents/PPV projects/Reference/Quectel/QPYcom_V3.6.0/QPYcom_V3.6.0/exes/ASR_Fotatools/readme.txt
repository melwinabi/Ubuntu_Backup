adiff.exe -l fp system_old.img system_new.img FotaFile.bin
adiff.exe system_old.img system_new.img dfota.bin 
adiff.exe system_old.img system_new.img dfota.bin -a1 customer_fs customer_fs.bin

最小系统升级：
adiff.exe system_old.img system_new.img dfota.bin [-a1 app1_patition_name app1_file_name [-a2 app2_patition_name app2_file_name]]

前三个参数是必选项， 
–a1 是可选项， –a2 是可选项，

-a1 代表当系统中存在和 system 同级的客户分区 app1，
分区名称为app1_patition_name，当要升级这个分区时可以使用这个参数，并将要升级的 app1_file_name 文 件 放 在 adiff.exe 同 一 路 径 下 ，
app1_file_name 文 件 在 工 具 制 作 时 会 被 放 入 dfota.bin_2 中，

在第二阶段升级，app1_patition_name 代表分区名称；app1_file_name 代表升级文件名称；
-a2 同上 a1，最小系统升级最多支持同时升级两个 app 分区。

在这里app1对应的是我们的customer_fs分区