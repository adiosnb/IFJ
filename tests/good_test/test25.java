// Pretypovanie pri operacich s int a double (mozno zle)

class Main
{
	static void run()
	{
		int a = 2.457878;
		double c = a * 2;

		int i = (c - 5) * 20;

		ifj16.print("a: " + a + "\n");
		ifj16.print("c: " + c + "\n");
		ifj16.print("Celkovo: " + i + "\n");
	}
}
