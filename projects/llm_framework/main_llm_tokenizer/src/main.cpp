/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>
#include <unistd.h>

int main()
{
    if (access("./_tokenizer.py", F_OK) == 0) {
        char *args[] = {"python3", "./_tokenizer.py", NULL};
        if (execvp("python3", args) == -1) {
            perror("execvp");
            return 1;
        }
    } else if (access("/opt/m5stack/share/_tokenizer.py", F_OK) == 0) {
        char *args[] = {"python3", "/opt/m5stack/share/_tokenizer.py", NULL};
        if (execvp("python3", args) == -1) {
            perror("execvp");
            return 1;
        }
    }
    perror("_tokenizer.py miss");
    return 0;
}