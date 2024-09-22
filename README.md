# **Client-Server Encode Decode with Error Detection and Correction**

This project implements a **client-server** system that processes string data with **error detection and correction** using **CRC** and **Hamming codes**. It demonstrates socket programming, multithreading, and error-handling in **C**.

## **Project Overview**

The main goal of this project is to create a **data processing system** that replaces lowercase vowels with uppercase vowels and appends the sum of all numbers found. The system utilizes **TCP socket communication** between a client and a server, supporting error detection via **CRC** and single-bit error correction via **Hamming codes**.

## **Key Features**

1. **Client-Server Architecture**: The client sends encoded data, and the server processes it through dedicated threads to manipulate characters and digits.
2. **Error Detection/Correction**: 
   - **CRC (Cyclic Redundancy Check)**: Detects transmission errors.
   - **Hamming Codes**: Detects and corrects single-bit errors.
3. **Multithreading**: The server uses seven threads to process different character replacements and sum the digits.
4. **Socket Communication**: Client and server communicate over TCP sockets, where data is sent, received, encoded, and decoded.

## **Implementation Details**

### **Client**:
The client reads from an input file (`intext.txt`) and offers three error mechanisms:
- **No Error**: Data is sent to the server without any errors.
- **CRC**: Users can choose to introduce errors in specific frames and bits.
- **Hamming**: Users can introduce errors in specific frames and bits. 

Upon receiving the response from the server, the client decodes the data and writes the result to `result.txt`.

### **Server**:
The server processes incoming client requests and uses **multithreading** to perform the following tasks:
- **Vowel Replacement**: Lowercase vowels (`a`, `e`, `i`, `o`, `u`) are replaced with their uppercase equivalents.
- **Digit Sum**: Calculates the sum of digits in the message.
- **Error Detection/Correction**:
  - CRC checks for errors but doesn’t correct them.
  - Hamming detects and corrects single-bit errors before processing the message.

### **High-Level Data Flow**:
- The server spawns two child processes: one for decoding and another for encoding.
- Seven threads are used for vowel replacement and digit sum calculation.
- The final encoded message is sent back to the client.

## **Command Line Arguments**

- **Client**: 
   `./client <server_ip> <server_port>`
   Reads data from `intext.txt`, encodes it, and sends it to the server.
   
- **Server**: 
   `./server <port>`
   Listens on the specified port for incoming client connections.

## **Error Handling**

1. **CRC**: Detects errors in transmission but does not correct them.
2. **Hamming Codes**: Detects and corrects single-bit errors.
   - If more than one bit is corrupted, an error message is shown.

## **How to Run the Project**

1. Clone the repository:
   `git clone <repository-url>`

2. Navigate to the project directory:
   `cd <project-directory>`

3. Compile the code:
   `gcc server.c -o server`
   `gcc client.c -o client`

4. Run the server:
   `./server <port>`

5. Open a new terminal and run the client:
   `./client <server_ip> <server_port>`

6. The client will read `intext.txt` and send the data to the server. The server will process the data, and the client will save the results to `result.txt`.

## **Testing**

1. Test the system with the provided input files, including the `intext.txt` file.
2. Introduce errors via the CRC and Hamming options to observe error detection and correction.
3. The results are saved in `result.txt`.

## **Technologies Used**

- **Programming Language**: C
- **Socket Programming**: Linux TCP Sockets
- **Error Detection/Correction**: CRC, Hamming Codes
- **Multithreading**: Pthread library

## **Documentation and Testing**

1. **System Documentation**: The project includes a **high-level data flow diagram**, descriptions of routines, and implementation details.
2. **Test Documentation**: Testing results are provided for different input files, including error detection and correction.
3. **User Documentation**: Instructions on how to run the program and input/output file descriptions are included.

