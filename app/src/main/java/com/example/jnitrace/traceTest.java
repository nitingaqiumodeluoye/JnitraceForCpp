package com.example.jnitrace;

/**
 * @author Zhenxi on 2022/1/15
 */
public class traceTest {

    public static String test() {
        return "test method is invoke ";
    }


    public static String test1(String str) {
        return "test method is invoke " + str;
    }

    public String test2(String str) {
        return "no static test method is invoke " + str;
    }


    public void test3(String str) {

    }

    public static int test4(String str) {
        return 555;
    }

    /**
     * 多参数回归测试。
     *
     * 专门用来验证 jnitrace 打印参数的值对不对:AArch64 的 va_list 是结构体,
     * 曾经因为按值传参导致推进丢失,每个参数都从第一个槽位重新读,
     * 打印出来全是同一个值。这里四个参数类型各不相同(String/int/long/boolean),
     * 取值方式也各不相同,能一次把这类错位暴露出来。
     */
    public static String test5(String str, int number, long bigNumber, boolean flag) {
        return "test5 " + str + " " + number + " " + bigNumber + " " + flag;
    }

}
