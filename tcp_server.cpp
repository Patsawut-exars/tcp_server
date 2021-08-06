#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <chrono>
#include <ctime>
#include <fstream>


/* データサイズ定義 */
#define BYTESIZE_uint8 1
#define BYTESIZE_uint16 2
#define BYTESIZE_double 8
#define BYTESIZE_boolean 1
/* データサイズ定義 */

/*  */


using namespace std;


/* 現在時間取得関数*/
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}
/* 現在時間取得関数 */

/* 現在時間取得関数(ms)：T.PATSAWUT*/
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
/* 現在時間取得関数(ms)：T.PATSAWUT*/
int main() {
    signal(SIGPIPE, SIG_IGN);
    int ln = socket(AF_INET, SOCK_STREAM, 0);//ソケットのアドレス
    int bytesize = BYTESIZE_double;
    
    if (ln < 0) {
        perror("socket() error");
        return -1;
    }
    struct sockaddr_in sa;//ソケット情報構成
    sa.sin_family = AF_INET;//IPアドレス
    sa.sin_port = htons(6000);//サーバーのポート番号
    sa.sin_addr.s_addr = htons(INADDR_ANY);//

    int one = 1;
    setsockopt(ln, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));//ソケット作成（設定も含む）
    if (bind(ln, (struct sockaddr*)&sa, sizeof(sa)) < 0)//ソケットにアドレス指定
     {
        perror("bind() error");
        return -1;
    }

    if (listen(ln, SOMAXCONN) < 0) //サーバーでLISTEN状態を開く
    {
        perror("listen() error");
        return -1;
    }

    cout << "Socked num: " << ln << endl;
    double buf[10] = {0};//受け取る変数定義（変数名、サイズ）
    double  bufb[5] = {1,2,3,4,5};//送り出す変数定義（変数名、サイズ）
    
    //ファイル変数の構造体宣言
    ofstream myFile_Handler;
    // Fileを開く
    myFile_Handler.open("log_file.txt");
    myFile_Handler << "データ送受信ログ" << endl;
    return 0;
    while (true) {
        cout << "Will accept" << endl;
        int conn = accept(ln, 0, 0);//Clientと接続ため関数
        if (conn < 0) {
            perror("accept() error");
        }

        int fail = false;
        while (!fail) {

            int n = read(conn, &buf, sizeof(buf));//MATLABから受信関数
            //int n = recv(conn, &buf, sizeof(buf) - 1, 0);
            if (n < 0) {
                perror("recv() error");
                break;//受信がエラーを出ると接続を切れる
            }
            if (n == 0) {
                break;//なにもこないと接続を切れる
            }
            buf[n] = '\0';
            //for (int i = 0;i < 10; i++)
           // {
                cout << "New data from client: " << *buf << endl;
           //}
            /* 現在時間取得 */
            //std::cout << currentDateTime() << std::endl;
            auto millisec=duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            std::cout<<"milliseconds:"<<millisec<<endl;
            /* 現在時間取得 */
                // ファイルに書き込む
            myFile_Handler << "データ：" << *buf<< ":"<<millisec<< endl;


            /* brake poinnt for debag */
            //break;
            /* brake poinnt for debag */
            int total = n;
            int written = 0;
            while (written < total + 1) {
                //sleep(10);//ディレー
                int n = write(conn,bufb,bytesize*5);//MATLABに送信関数
                //int n = send(conn, buf + written, total + 1 - written, 0);
                if (n < 0) 
                {
                    perror("send() error");
                    fail = true;
                    break;//送信がエラーを出ると送信を終了して接続を切れる
                }
                written += n;
            }
        }
        cout << "Client disconnected" << endl;
            
    
    }
    // Fileを閉じる
    myFile_Handler.close();
    return 0;
}



