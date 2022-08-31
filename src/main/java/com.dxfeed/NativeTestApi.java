package com.dxfeed;

import com.oracle.svm.core.c.ProjectHeaderFile;
import org.graalvm.nativeimage.IsolateThread;
import org.graalvm.nativeimage.UnmanagedMemory;
import org.graalvm.nativeimage.c.CContext;
import org.graalvm.nativeimage.c.constant.CEnum;
import org.graalvm.nativeimage.c.constant.CEnumLookup;
import org.graalvm.nativeimage.c.constant.CEnumValue;
import org.graalvm.nativeimage.c.function.CEntryPoint;
import org.graalvm.nativeimage.c.function.CFunctionPointer;
import org.graalvm.nativeimage.c.function.InvokeCFunctionPointer;
import org.graalvm.nativeimage.c.struct.*;
import org.graalvm.nativeimage.c.type.CCharPointer;
import org.graalvm.nativeimage.c.type.CTypeConversion;
import org.graalvm.word.PointerBase;

import java.nio.charset.StandardCharsets;
import java.util.Collections;
import java.util.List;

@CContext(NativeTestApi.NativeTestApiDirectives.class)
public class NativeTestApi {
    static class NativeTestApiDirectives implements CContext.Directives {
        @Override
        public List<String> getHeaderFiles() {
            /*
             * The header file with the C declarations that are imported. We use a helper class that
             * locates the file in our project structure.
             */
            return Collections.singletonList(ProjectHeaderFile.resolve("com.dxfeed", "native/dxf_types.h"));
        }
    }


    @CEnum("dxf_event_type_t")
    enum EventsTypes {
        DXF_EVENT_TYPE_QUOTE,
        DXF_EVENT_TYPE_TIME_AND_SALE,
        DXF_EVENT_TYPE_CANDLES;

        @CEnumValue
        public native int getCValue();

        @CEnumLookup
        public static native EventsTypes fromCValue(int value);
    }

    /**
     * Base type of all events.
     */
    @CStruct("dxf_event_t")
    interface EventNative extends PointerBase {
        @CField("event_type")
        int getEventType();

        @CField("event_type")
        void setEventType(int eventType);

        @CField("symbol_name")
        CCharPointer getSymbolName();

        @CField("symbol_name")
        void setSymbolName(CCharPointer symbolName);
    }

    /**
     * Pointer to EventNative.
     */
    @CPointerTo(EventNative.class)
    public interface EventNativePtr extends PointerBase {
        /**
         * Provides access to an array element by index.
         *
         * @param index The index of array.
         * @return Returns pointer.
         */
        EventNativePtr addressOf(int index);

        EventNative read();

        void write(EventNative data);
    }

    /**
     * The Quote.
     */
    @CStruct("dxf_event_quote_t")
    interface QuoteNative extends EventNative {
        @CField("bid_price")
        double getBidPrice();

        @CField("bid_price")
        void setBidPrice(double bidPrice);

        @CField("ask_price")
        double getAskPrice();

        @CField("ask_price")
        void setAskPrice(double askPrice);
    }

    /**
     * The TimeAndSale.
     */
    @CStruct("dxf_event_time_and_sale_t")
    interface TimeAndSaleNative extends EventNative {
        @CField("event_flag")
        int getEventFlag();

        @CField("event_flag")
        void setEventFlag(int eventFlag);

        @CField("index")
        long getIndex();

        @CField("index")
        void setIndex(long index);
    }

    interface EventListenerFunctionPtr extends CFunctionPointer {
        @InvokeCFunctionPointer
        void invoke(EventNativePtr events, int count);
    }

    @CEntryPoint(name = "native_create_events_and_call_listener")
    public static void createEventsAndCallListener(IsolateThread ignoreIsolate, EventListenerFunctionPtr listener) {
        // Allocates an array of pointers.
        var events = (EventNativePtr) UnmanagedMemory.calloc(SizeOf.get(EventNativePtr.class) * 2);

        // Allocates Quote events.
        var quote = (QuoteNative) UnmanagedMemory.calloc(SizeOf.get(QuoteNative.class));
        quote.setEventType(EventsTypes.DXF_EVENT_TYPE_QUOTE.getCValue());
        quote.setSymbolName(allocCString("😁"));
        quote.setAskPrice(1);
        quote.setBidPrice(2);

        // Allocates Quote events.
        var tns = (TimeAndSaleNative) UnmanagedMemory.calloc(SizeOf.get(TimeAndSaleNative.class));
        tns.setEventType(EventsTypes.DXF_EVENT_TYPE_TIME_AND_SALE.getCValue());
        tns.setSymbolName(allocCString("ETH/USD:GDAX"));
        tns.setEventFlag(3);
        tns.setIndex(4);

        // Writes event pointers to array of pointers.
        events.addressOf(0).write(quote);
        events.addressOf(1).write(tns);

        // Calls C listener.
        listener.invoke(events, 2);

        // Free all allocated memory.
        UnmanagedMemory.free(tns.getSymbolName());
        UnmanagedMemory.free(tns);
        UnmanagedMemory.free(quote.getSymbolName());
        UnmanagedMemory.free(quote);
        UnmanagedMemory.free(events);
    }

    private static CCharPointer allocCString(String string) {
        var bytes = string.getBytes(StandardCharsets.UTF_8);
        var pointer = (CCharPointer) UnmanagedMemory.calloc(bytes.length + 1);
        for (int i = 0; i < bytes.length; ++i) {
            pointer.addressOf(i).write(bytes[i]);
        }
        return pointer;
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
