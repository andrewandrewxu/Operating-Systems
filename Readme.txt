Name: Xu Zhaoxuan
Student Number: V00791274

First, the diskinfo and disklist work perfectly without any problem. I just went into the disk accordingly and read the information.

Diskget also works very well except a little formatting, but I implemented all the functionalities.

For Diskput, I simply go through the FAT table and record the available ones, and put the data into the according physical sectors, and assign the next FAT index into the previous FAT value. Also, when access the free FAT 1st time, assign the directory stuff, including: File size, 1st logical cluster, and  file name and extension.

SOmetimes, the file I put into the disk has some invalid chars at the very end, but the whole content is avalible and you surely can access the file. It just happens rarely. 