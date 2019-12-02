#include <stdarg.h>     /* va_list */
#include <stdio.h>
#include <stdlib.h>     /* exit() */
#include <sys/ptrace.h>
#include <sys/types.h>  /* pid_t */
#include <sys/wait.h>   /* wait() */
#include <unistd.h>     /* fork() */
#include <sys/user.h>   /* struct user_regs_struct */

void print_error(const char *message) {
    perror(message);
    exit(1);
}
void print_message(const char *message, ...) {
    va_list args_list;
    va_start(args_list, message);
    vfprintf(stdout, message, args_list);
    va_end(args_list);
}

void start_target(char *program_name) {
    print_message("Function 'start_target'started\n");

    /* Allow to parent trace this process */
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
        print_error("[start_target] ptrace\n");
        return;
    }

    /* Replace this process's image with given program */
    execl(program_name, program_name, NULL);
}
void start_debugger(pid_t child_pid) {
    print_message("Function 'start_debugger' started\n");

    int wait_status;
    unsigned counter = 0;

    /**
     * Wait for child to change state.
     * In our case: child stops on its first instruction
     */
    wait(&wait_status);

    while (WIFSTOPPED(wait_status)) {

        counter++;

        struct user_regs_struct regs;

        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);

        unsigned instruction = ptrace(PTRACE_PEEKTEXT, child_pid, regs.rip, 0);
        print_message("counter = %u, RIP = 0x%08x, intruction = 0x%08x\n",
                      counter, regs.rip, instruction);

        /* Child execute another instuction */
        if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) {
            print_error("[start_debugger] ptrace\n");
            return;
        }

        wait(&wait_status);
    }

    print_message("The child executed %u instructions\n", counter);
}

int main(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "Please provide a programm name\n");
        return -1;
    }

    pid_t child_pid;

    /* Try to create a child process */
    child_pid = fork();

    if (child_pid == 0) {
        print_message("Run function for child\n");
        start_target(argv[1]);

    } else if (child_pid > 0) {
        print_message("Run function for parent\n");
        start_debugger(child_pid);
    } else {
        print_error("fork()");
    }
}