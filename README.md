# File Transfer Server/Client

This project implement a file transfer server and client. It also embedded with huffman compression algorithm that decrease the file size along the transfer.

## Execution
server: The PORT on server is pre-defined as 666
```
make
./server
```
client:
```
make
./client hostname PORT // example ./client MBP 666
Enter file name:
```

## TODO
- [x] Huffman Compression
- [x] Face Detection
- [ ] Invoke TF model in C