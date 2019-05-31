import java.io.IOException;
import java.util.Random;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.lang.Math;

public class main {

    public static final int expire_time = 10;
    public static final int process_size = 12;
    public static final int page_size = 80;
    public static final int process_seq = 120;
    public static int value_fifo = 0, value_opt = 0, value_lru = 0;


    public static void main(String args[]) throws IOException {

        BufferedWriter stream_out = new BufferedWriter(new FileWriter("output.csv"));
        stream_out.write("# of Process," + process_size + "\n");
        stream_out.write("# of Page," + page_size + "\n");
        stream_out.write("Page fault occurrence\n");
        stream_out.write("Round,FIFO,OPT,LRU\n");
        int sum_fifo = 0, sum_opt = 0, sum_lru = 0;
        Random rand = new Random();

        for (int count = 0; count < expire_time; count++) {
            value_fifo = 0;
            value_opt = 0;
            value_lru = 0;

            int[] p = new int[process_size];
            int[] page = new int[page_size];
            int[] def_p = new int[process_size];

            int[] def_p_new = {1,2,3,4,1,2,5,1,2,3,4,5};
            for (int i = 0 ; i < 12 ; i++ ){
                def_p[i] = def_p_new[i];
            } 


            /////////////////// type 1 
            for (int i = 0; i < process_size ; i++) { 
                int x = Math.abs(rand.nextInt()) % process_seq;
                while (x == def_p[i]) { 
                    x = Math.abs(rand.nextInt()) % process_seq; 
                }
                def_p[i] = x; 
            }
             

  /*          ////////////////// type 2 
            int[] def_p = new int[process_size];
            int prev = -1;
            int rerand = 0;
            for (int i = 0; i < process_size; i++) {
                int x = Math.abs(rand.nextInt()) % process_seq;
                boolean[] flag = new boolean[process_seq];
                boolean abb = true;
                if (i % process_seq == 0) {
                    for (int k = 0; k < process_seq; k++) {
                        flag[k] = false;
                    }
                }
                if (flag[x] || rerand == prev) {
                    rerand = Math.abs(rand.nextInt()) % process_seq;
                    while (abb) {
                        if (flag[rerand] || rerand == prev) {
                            rerand = Math.abs(rand.nextInt()) % process_seq;
                        } else {
                            abb = false;
                        }
                    }
                    def_p[i] = rerand;
                    flag[rerand] = true;
                    prev = rerand;
                } else {
                    def_p[i] = x;
                    flag[x] = true;
                    prev = x;
                }
            }
*/

            page = Reset(page);
            p = Set_p(def_p);


            FIFO(page, p);
            Optimal(page, p);
            LRU(page, p);


            sum_fifo += value_fifo;
            sum_opt += value_opt;
            sum_lru += value_lru;

            stream_out.write(count + 1 + "," + value_fifo + "," + value_opt + "," + value_lru + "\n");
        }
        stream_out.write("AVG : ," + sum_fifo / expire_time + "," + sum_opt / expire_time + "," + sum_lru / expire_time);
        stream_out.close();
    }


    public static int[] Reset(int[] page) {
        for (int i = 0; i < page_size; i++) {
            page[i] = -1;
        }
        return page;
    }

    public static int[] Set_p(int[] def_p) {
        int[] k = new int[process_size];
        for (int i = 0; i < process_size ; i++) {
            k[i] = def_p[i];
        }
        return k;
    }
        //////////// function FIFO 
    public static void FIFO(int[] page, int[] p) {
        int count = 0;
        //////////// loop ตามจำนวน process 
        for (int i = 0; i < process_size; i++) {
            if (count >= page_size){
                count = 0;
            }

            int checknum = Find_page(page, p[i]);

            if ((page[count] < 0 && checknum == -1) || checknum == -1) {
                page[count] = p[i];
                count++;
                value_fifo++;
            }
        }
        System.out.println("FIFO page_falut : " + value_fifo);
    }

        //////////////// function  Optimal 
    public static void Optimal(int[] page, int[] p) {

        int max = -1;
        int[] x = new int[page_size];
        int idx = -1;

        for(int i=0 ; i<page_size ; i++){
            page[i] = p[i] ;
            value_opt++;
        }

        for (int i = page_size; i < process_size; i++) {
            max = -1;
            idx = -1;
            for (int j = 0; j < page_size; j++) {
                x[j] = -1;
            }

            if (Find_page(page, p[i]) < 0) {

                for (int k = i; k < process_size ; k++) {
                    int count = Find_page(page, p[k]);
                    if (k > idx && count >= 0) {
                        if (x[count] == -1) {
                            idx = k;
                            max = count;
                            x[count] = count;
                        }
                    }
                }
                for (int k = 0; k < page_size; k++) {
                    if (x[k] < 0) {
                        max = k;
                    }
                }
                if (max == 1) {
                    max = 0;
                }
                page[max] = p[i];
                value_opt++;
            }
        }
        System.out.println("Optimal page_fault : " + value_opt);
    }
    /////////// function LRU 
    public static void LRU(int[] page, int[] p) {

        int idx = 0;
        int checknum = 0;
        int[] history = new int[page_size];

        for (int i = 0; i < page_size; i++) {
            history[i] = 0;
            page[i] = p[i];
            history[i] += i;
            value_lru++;
        }

        for (int i = page_size; i < process_size; i++) {

            checknum = Find_page(page, p[i]);

            if (checknum >= 0) {
                history[checknum] = i;
            } else {

                for (int j = 0; j < page_size; j++) {
                    int min = history[j];
                    for (int k = 0; k < page_size; k++) {

                        if (history[k] < min) {
                            idx = k;
                            min = history[k];
                        }
                    }
                }
                page[idx] = p[i];
                history[idx] = i;
                value_lru++;
            }
        }
        System.out.println("LRU page_falut : " + value_lru + "\n");
    }
        ////////////////// function ดูว่า มี process อยุ่ใน page นี้หรือไม่ 
    public static int Find_page(int[] page, int p) {
        for (int i = 0; i < page_size; i++) {
            if (page[i] == p)
                return i;
        }
        //////////// ไม่มี return -1 
        return -1;
    }

}