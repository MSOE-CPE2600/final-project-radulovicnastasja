#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define RED_TEXT "\033[38;5;196m"
#define YELLOW_TEXT "\033[38;5;226m"
#define RESET_TEXT "\033[0m"  // Reset color

// Function to handle Markdown-like formatting for colored text
void process_markdown(char *input, char *output) {
    int i = 0, j = 0;
    while (input[i] != '\0') {
        // For "bold" text (we'll treat it as red text with **)
        if (input[i] == '*' && input[i + 1] == '*' && (i == 0 || input[i - 1] != '*')) {
            strcat(output, RED_TEXT);  // Start red text
            i += 2;  // Skip the "**"
        } else if (input[i] == '*' && input[i + 1] == '*' && input[i - 1] != '*') {
            strcat(output, RESET_TEXT);    // End red text (reset color)
            i += 2;  // Skip the "**"
        }
        // For "italic" text (we'll treat it as yellow text with *)
        else if (input[i] == '*' && (i == 0 || input[i - 1] != '*')) {
            strcat(output, YELLOW_TEXT);  // Start yellow text
            i++;  // Skip the "*"
        } else if (input[i] == '*' && input[i - 1] != '*') {
            strcat(output, RESET_TEXT);    // End yellow text (reset color)
            i++;  // Skip the "*"
        } else {
            output[j++] = input[i++];  // Normal character
        }
    }
    output[j] = '\0';
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert address
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    printf("Connected to the server.\n");

    // Chat loop
    while (1) {
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sock, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "/exit\n") == 0) {
            printf("Chat closed by client.\n");
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(sock, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        char formatted_message[BUFFER_SIZE] = {0};
        process_markdown(buffer, formatted_message);

        printf("Server: %s", formatted_message);

        if (strcmp(buffer, "/exit\n") == 0) {
            printf("Chat closed by server.\n");
            break;
        }
    }

    close(sock);
    return 0;
}
