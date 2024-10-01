#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include<sys/wait.h>
#include <math.h>
#include "encDec.h"

#define BUFFER_SIZE 300001

void crc_division(char *dividend, const char *divisor, int len_dividend, int len_divisor) {
    for (int i = 0; i <= len_dividend - len_divisor; i++) {
        if (dividend[i] == '1') {
            for (int j = 0; j < len_divisor; j++) {
                dividend[i + j] = (dividend[i + j] == divisor[j]) ? '0' : '1';
            }
        }
    }
}

int is_power_of_two(int x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}

int calculate_parity_bit(char *data, int position, int m) {
    int parity = 0;
    for (int i = position - 1; i < m; i += 2 * position) {
        for (int j = i; j < i + position && j < m; j++) {
            parity ^= (data[j] - '0');
        }
    }
    return parity;
}

void hamming_encode(char *data, int n, char *encoded_data, int m) {
    for (int i = 0, j = 0; i < m; i++) {
        if (is_power_of_two(i + 1)) {
            encoded_data[i] = '0';
        } else {
            encoded_data[i] = data[j++];
        }
    }

    for (int i = 0; i < m; i++) {
        if (is_power_of_two(i + 1)) {
            encoded_data[i] = calculate_parity_bit(encoded_data, i + 1, m) + '0';
        }
    }
}



int main(int argc, char *argv[]) {
    
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];
    FILE *fp_in, *fp_out;
    long file_size;
    char error;
    int position;
    char crcstring[]="100000100110000010001110110110111";

    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, 
          (char *) &serv_addr.sin_addr.s_addr, 
          server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    ///////


//logic for choosing crc or hamming
char crc_hamming;
int flag;
printf("Do you want to use CRC or Hamming y/n\n");
scanf("%c",&crc_hamming);
if(tolower(crc_hamming)=='y'){
    printf("Do you want to use CRC or Hamming \n 1:CRC, 2: Hamming\n 1/2:");
    scanf("%d",&flag);
}
//printf("%d",flag);
write(sockfd,&flag,sizeof(int));


//1st process
pid_t pid;

pid=fork();

if(pid<0){
    fprintf(stderr,"Fork failed\n");
    return 1;
}
else if(pid==0){


// clientencoder
    char* filename = "intext.txt";
    off_t offset = 0;
    char* ascii_array = NULL;
    int buffer_size=32;
    int binFd = open("input.binf", O_CREAT | O_WRONLY, 0644);
    
    int count=0;
    
    while ((ascii_array = read_file(filename, &offset,buffer_size)) != NULL) {
        count+=1;
        
        char binary_array[281]={'\0'};
        char arr1[9]={'\0'};
        char* binary1=int_to_binary(22);
        strcpy(arr1,binary1);
            
            
        strcat(binary_array,arr1);
        
        char arr2[9]={'\0'};
        char* binary2=int_to_binary(22);
        strcpy(arr2,binary2);
           
            
        strcat(binary_array,arr2);
       

        char arr3[9]={'\0'};
        char* binary3=int_to_binary((int)strlen(ascii_array));
        strcpy(arr3,binary3);
            
            
        strcat(binary_array,arr3);
        //printf("only 2222 and length%s\n",binary_array);
        //printf("%ld\n",strlen(binary_array));

        

        //storing framed ascii values into binary strings in binary_array and have null character
        for (int i =0; i < strlen(ascii_array); i++) {
            char* binary = ascii_to_binary(ascii_array[i]);
            binary=add_odd_parity(binary);
            char arr[9]={'\0'};

            strcpy(arr,binary);
            strcat(binary_array,arr);
            //printf(" appending each caharacter%s\n",binary_array);
            //printf("%ld\n",strlen(binary_array));
        }





if(flag==1){
    
    int len_binstring=strlen(binary_array);
    int len_crcstring=strlen(crcstring);
        // printf("binary_array len %d\n",&len_frame);
        // printf("crc_array len %d\n",&len_crc);
        // Allocate enough space for the binstring_with_remainder 
    char binstring_with_remainder[len_binstring + len_crcstring-1];
    strcpy(binstring_with_remainder, binary_array);
    char modified_binstring[len_binstring + len_crcstring];
    strcpy(modified_binstring, binstring_with_remainder);
    for (int i = 0; i < len_crcstring - 1; i++) {
        modified_binstring[len_binstring + i] = '0';
    }
    modified_binstring[len_binstring + len_crcstring - 1] = '\0';

    crc_division(modified_binstring, crcstring, len_binstring + len_crcstring - 1, len_crcstring);
        
    char remainder[len_crcstring];
    strncpy(remainder, &modified_binstring[len_binstring], len_crcstring - 1);
    remainder[len_crcstring - 1] = '\0';

    //printf("CRC remainder: %s\n", remainder);  
        
    strcat(binstring_with_remainder, remainder);    
        
    printf("Do you want to introduce error in frame %d: y/n \n",count);
    scanf(" %c", &error);
    if (tolower(error) == 'y') {
        printf("Enter a data position (0 to 311) where to enter error \n");
        scanf("%d", &position);
        if (position >= 0 && position < strlen(binstring_with_remainder)) {
            binstring_with_remainder[position] = (binstring_with_remainder[position] == '0') ? '1' : '0';
            printf("Bit flipped at position %d\n", position);
        } else {
            printf("Invalid position. The binstring remains unchanged.\n");
        }
    }  
        
    write(binFd,binstring_with_remainder,312);
    
    }
    else if(flag==2){

        //Hamming encoding logic code
        //binary array has data now
        char data[256];
        strcpy(data,&binary_array[24]);
        int n = strlen(data);
        int p=0;
         while ((1 << p) < (n + p + 1)) {
        p++;
        }
        int m = n + p;

        char *encoded_data = (char *)calloc(m + 1, sizeof(char));
        hamming_encode(data, n, encoded_data, m);

        //printf("Encoded message using hamming: %s\n", encoded_data);

        // // Introduce an error
        // encoded_data[3] ^= 1;
        // printf("Encoded data with error: ");
        // for (int i = 0; i < m; i++) {
        //     printf("%d", encoded_data[i]);
        // }
        // printf("\n");
        char hamencoded_message[265];
        strcpy(hamencoded_message,encoded_data);

        printf("Do you want to introduce error in frame %d: y/n \n",count);
        scanf(" %c", &error);
        if (tolower(error) == 'y') {
            printf("Enter a data position (0 to %ld) where to enter error \n",strlen(hamencoded_message));
            scanf("%d", &position);
            if (position >= 0 && position < strlen(hamencoded_message)) {
                hamencoded_message[position] = (hamencoded_message[position] == '0') ? '1' : '0';
                printf("Bit flipped at position %d\n", position);
            } else {
                printf("Invalid position. The hamencoded_message remains unchanged.\n");
            }
        }

        char hamencoded_frame[290];
        int prefix_length = 24;
        int binary_array_length = strlen(binary_array);
        int hamencoded_message_length = strlen(hamencoded_message);

        strncpy(hamencoded_frame, binary_array, prefix_length);
        strcpy(hamencoded_frame + prefix_length,hamencoded_message);

        //printf("Hamming encoded frame of length %ld: \n %s\n", strlen(hamencoded_frame),hamencoded_frame);


        write(binFd,hamencoded_frame,289);
        free(encoded_data);

    }


    else{
        write(binFd,binary_array,280);
    }
        
        //write(child_to_parent_pipe[WRITE_END],binary_array,280);
        //printf("%s\n",binary_array);
        //printf("%ld\n",strlen(binary_array));

        if ((int)strlen(ascii_array) < buffer_size) {
            break;
        }
        free(ascii_array);
    }
    close(binFd);

    //close(child_to_parent_pipe[WRITE_END]);










    ///////////

    fp_in = fopen("input.binf", "r");
    if (fp_in == NULL) {
        perror("ERROR opening binf file");
        exit(1);
    }

    // get file size
    fseek(fp_in, 0L, SEEK_END);
    file_size = ftell(fp_in);
    fseek(fp_in, 0L, SEEK_SET);

    // read entire file into buffer
    char file_contents[file_size + 1];
    fread(file_contents, sizeof(char), file_size, fp_in);
    file_contents[file_size] = '\0';

    // send file contents over socket connection
    n = write(sockfd, file_contents, strlen(file_contents));
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    bzero(buffer, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    fp_out = fopen("read_input.chck", "w");
    if (fp_out == NULL) {
        perror("ERROR opening output file");
        exit(1);
    }

    fprintf(fp_out, "%s", buffer);

    fclose(fp_in);
    fclose(fp_out);
    close(sockfd);


}else{



wait(NULL);



//2nd process


    ///client decoder
    // FILE *fp;
    // fp=fopen("read_input_chck.chck","w");
    // fputs(buffer,fp);
    // fclose(fp);
    char* read_input_chck="read_input.chck";
    off_t offset2=0;
    char* read_encoded_array=NULL;
    int read_encoded_array_size=280;
    int inpDn = open("result.txt", O_CREAT | O_WRONLY, 0644);

while ((read_encoded_array = read_file(read_input_chck, &offset2,read_encoded_array_size)) != NULL) {


            //not considering first 18 bits from encoded array
            char* new_read_encoded_array=read_encoded_array+24;
            // length of string
            int new_encoded_len = strlen(new_read_encoded_array);

            int num_chunks = new_encoded_len / 8;

            for (int i = 0; i < num_chunks; i++) {
            char* read_chunk=malloc(sizeof(char)*9);
            strncpy(read_chunk, new_read_encoded_array + i*8, 8);
            read_chunk[8] = '\0';

            char* binary_without_parity=remove_odd_parity(read_chunk);

            char c=binary_to_ascii(binary_without_parity);
            
            //c=toupper(c);

            write(inpDn,&c,sizeof(char));
            free(read_chunk);
            }

            if ((int)strlen(read_encoded_array) < read_encoded_array_size) {
                break;
            }
            free(read_encoded_array);
        }


    off_t file_size = lseek(inpDn, 0, SEEK_END);
    // if (file_size == (off_t)-1) {
    //     perror("lseek");
    //     close(fd);
    //     exit(1);
    // }

    if (file_size == 0) {
        printf(" Result file is empty as error is introduced and crc is failed at server\n");
    } 

    close(inpDn);






}

    return 0;
}



