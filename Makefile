make:
	gcc -o test test_utils.c utils.c
	gcc -o webserver webserver.c utils.c -lws2_32