

int send_char( char c ) {
	//startbit.
	// pin high
	do {
		//delay.
		if (c&1)
			//set pin high.
		else
			//set pin low.
	} while (c>>=1)
	// delay
	//stopbit.
	// pin low
	// delay
}
