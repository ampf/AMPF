/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 ******************************************************************************/
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;

/**
 * A simple process wrapper that forks a sub-process for wrapped buffers.
 * @author stuffs
 *
 */
public class Wrapper {

    static PrintStream nullout = new PrintStream(System.out) {
        public void println(String x) {}
        public void print(String x) {}
    };

    static PrintStream stdout = nullout;

    @SuppressWarnings("unchecked")
    public static void main(String args[]) throws Exception {

        if (new Boolean(System.getProperty("mpf-debug", "false")).booleanValue()) {
            stdout = System.out;
        }

        if (args.length < 3) {
            stdout.println("usage: Wrapper sink src command");
            return;
        }
        FileInputStream fis = new FileInputStream(args[0]);
        DataInputStream dis = new DataInputStream(fis);
        FileOutputStream fos = new FileOutputStream(args[1]);
        DataOutputStream dos = new DataOutputStream(fos);

        String command = args[2];

        command = command + " &";

        int index = 0;
        try {
            while (true) {

                int size = dis.readInt();
                long timestamp = dis.readLong();
                long duration = dis.readLong();

                stdout.println("Wrapper: size=" + size + " timestamp=" + timestamp + " (0x" + Long.toHexString(timestamp) + ")" + " duration=" + duration + " (0x" + Long.toHexString(duration) + ")");

                byte buffer[] = new byte[size];
                dis.read(buffer);

                stdout.println("Wrapper: input=" + new String(buffer));

                // Fork command, send buffer to input, gather output, and return it.
                stdout.println("Wrapper: forking " + command);
                Process p = Runtime.getRuntime().exec(new String[]{"sh", "-c", command});

                FileOutputStream fos2 = new FileOutputStream(args[3]);
                FileInputStream fis2 = new FileInputStream(args[4]);

                stdout.println("Wrapper: shoveling data");

                StringBuffer out = new StringBuffer(), err = new StringBuffer();
                Thread tout = dump(fis2, null, out, "out");
                Thread terr = dump(p.getErrorStream(), null, err, null);
                Thread tin = dump(new ByteArrayInputStream(buffer), fos2, null, null);

                p.waitFor();
                tout.join();
                /*
                terr.join();
                tin.join();
                */

                String string = out.toString();
                stdout.println("Wrapper: output=" + string);
                dos.writeInt(string.length());
                dos.writeLong(timestamp);
                dos.writeLong(-1);
                dos.write(string.getBytes());

                index++;
            }
        } catch (IOException iox) {
            // Done.
            stdout.println("done.");
        }
    }

    public static Thread dump(final InputStream s, final OutputStream o, final StringBuffer buf, final String show) {
        Thread t = new Thread() {
            boolean first = true;
            public void run() {
                try {
                    while(true) {
                        int c = s.read();
                        if (c == -1)
                            break;
                        if (show != null) {
                            if (first) {
                                first = false;
                                stdout.print(show + ">>> ");
                            } else {
                                stdout.print((char)c);
                            }
                            stdout.flush();
                            if (c == '\n') {
                                first = true;
                            }
                        }
                        if (o != null)
                            o.write(c);
                        if (buf != null)
                            buf.append((char)c);
                    }
                    o.close();
                } catch (Exception x) {
                }
            }
        };
        t.setPriority(Thread.MAX_PRIORITY);
        t.start();
        return t;
    }
}
