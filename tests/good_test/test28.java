// Rekurzia

class Main
{
	static int a = 10;

	static void run()
	{
		int v = Druha.rekurz();
		ifj16.print("Vysledok: " + v + "\n");
	}
}

class Druha
{
	static int b = Main.a;
	static int rekurz()
	{
	int tmp;
		if (b > 5)
		{
			b = b - 1;
			ifj16.print("b: " + b + "\n");
			tmp = Druha.rekurz();
			return tmp;
		}
		else  
		{

		}

		return 258;
	}
}
