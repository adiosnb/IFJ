//missing return value in non-void function #8
class Main {
    static void run() {
         int a = add(5, 10);
    }

    static int add(int a, int b){
        return;
    }

}
