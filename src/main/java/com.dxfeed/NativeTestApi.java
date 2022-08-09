package com.dxfeed;

import org.graalvm.nativeimage.IsolateThread;
import org.graalvm.nativeimage.c.CContext;
import org.graalvm.nativeimage.c.function.CEntryPoint;
import org.graalvm.nativeimage.c.type.CCharPointer;
import org.graalvm.nativeimage.c.type.CTypeConversion;

@CContext(NativeTestApi.NativeTestApiDirectives.class)
public class NativeTestApi {
    static class NativeTestApiDirectives implements CContext.Directives {
    }

    /**
     * Creates graalvm native thread.
     *
     * @return The graalvm native thread.
     */
    @CEntryPoint(name = "native_test_api_create_isolate", builtin = CEntryPoint.Builtin.CREATE_ISOLATE)
    static native IsolateThread createIsolate();

    /**
     * Adds two integer numbers.
     *
     * @param thread The graalvm native thread.
     * @param a      The first number.
     * @param b      The seconds number.
     * @return Returns sum of two numbers.
     */
    @CEntryPoint(name = "native_test_api_add")
    static int add(final IsolateThread thread, int a, int b) {
        return a + b;
    }

    /**
     * Sets Java System Property as key-value pair.
     *
     * @param thread The graalvm native thread.
     * @param key    The key.
     * @param value  The value.
     */
    @CEntryPoint(name = "native_test_set_system_property")
    static void setSystemProperty(final IsolateThread thread, final CCharPointer key, final CCharPointer value) {
        System.setProperty(CTypeConversion.toJavaString(key), CTypeConversion.toJavaString(value));
    }

    /**
     * Checks Java System Property by key.
     *
     * @param thread The graalvm native thread.
     * @param key    The key.
     * @param value  The value.
     * @return {@code true} if the passing key-value equivalent to the corresponding Java System Property,
     * {@code false} otherwise.
     */
    @CEntryPoint(name = "native_test_check_system_property")
    static boolean checkSystemProperty(final IsolateThread thread, final CCharPointer key, final CCharPointer value) {
        var property = System.getProperty(CTypeConversion.toJavaString(key));
        return property.equals(CTypeConversion.toJavaString(value));
    }
}
