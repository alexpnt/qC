char buffer[20];
int main(int argc, char **argv) {	
	int a,b;	
	a = atoi(argv[1]);
	b = atoi(argv[2]);

	if (a == 0) {
		printf(itoa(b, buffer));
		printf("\n");

	}
	else{
		while (b > 0)
			if (a > b)
				a = a-b;
			else
				b = b-a;
		printf(itoa(a, buffer));
		printf("\n");
	}
	
	return 0;
	
}
