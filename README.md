# Compose-A-Unix-Shell
operating system course project.

# This project can compile and run correctly by simlpy running the following commands:

	$ tar -xzvf project.tgz
  
	$ cd project
  
	$ make
  
	$ ./shell


• The shell prints a prompt when it is ready for the user to input a command. 

  - $ 
  
• Background and foreground running.

  - $ ls -l | wc &
  
    [3256]
      
    $
  
• When a sub-command is not found, a proper error message should be displayed, immediately

  followed by the next prompt.
  
• Input and output redirection

  - $ wc < valid_file.txt
  
  - $ ls -l > a.txt
  
# see more in shell.pdf
