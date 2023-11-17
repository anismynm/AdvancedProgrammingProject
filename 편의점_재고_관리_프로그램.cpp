#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

string goodsInfo[100][100]; // main 함수와 클래스 구현에서 모두 공용으로 사용해야 하는 배열이라 전역 변수 선언.

class Goods {
private:
    int goodsNum;
public:
    int CheckedDay;

    Goods() {
        goodsNum = 0;
    }

    Goods(int day, int CheckedDay, int goodsNum) {
        this -> goodsNum = goodsNum;
        this -> CheckedDay = CheckedDay;
        string fileName = to_string(day) + ".txt";
        ifstream inputFile(fileName);

        if (!inputFile.is_open()) {
            cout << "* 재고 파일 열기에 실패하였습니다!" << endl;
            return;
        }
        
        for (int i = 0; i < goodsNum + 1; i ++) {
            string dump;
            getline(inputFile, dump, '|'); // 줄 바꿀 때마다 나오는 덤프 값 버리기
            for (int j = 0; j < CheckedDay + 3; j ++) {
                getline(inputFile, goodsInfo[i][j], '|');
            }
        }

        cout << "* " << day / 100 << "월 " << day % 100 << "일 " << "기준으로 재고 파일 불러오기 완료." << endl;

        inputFile.close();
    }

    void DailyCheck(int day) {
        ifstream inputFile(to_string(day - 1) + ".txt");
        ofstream outputFile(to_string(day) + ".txt");
        string line;

        if (!inputFile.is_open() || !outputFile.is_open()) {
            cout << "* 파일 저장에 실패하였습니다." << endl;
            return;
        }

        getline(inputFile, line);
        goodsInfo[0][CheckedDay + 3] = to_string(day);
        outputFile << line << to_string(day) << '|' << endl;

        cout << "* " << day / 100 << "월 " << day % 100 << "일 " << "재고를 등록합니다." << endl;

        for (int i = 1; i < goodsNum + 1; i ++) {
            cout << endl << "* 상품정보 : ";
            for (int j = 0; j < 2; j ++) {
                cout << goodsInfo[i][j] << ", ";
            }
            cout << "전날 재고 : " << goodsInfo[i][CheckedDay + 2] << endl;
            cout << "* 금일 재고 입력 >> ";
            rewind(stdin);
            cin >> goodsInfo[i][CheckedDay + 3];

            getline(inputFile, line);
            outputFile << line << goodsInfo[i][CheckedDay + 3] << '|' << endl;
        }

        inputFile.close();
        outputFile.close();

        cout << endl << "* " << day / 100 << "월 " << day % 100 << "일 " << "판매량 등록이 완료되어 " << to_string(day) << ".txt 파일로 저장되었습니다." << endl;

    }

    void TotalSales(int day) {
        int saleSum = 0;
        int snackSum = 0;
        int beverageSum = 0;
        for (int i = 1; i < goodsNum + 1; i ++) {
            int goodsSale = stoi(goodsInfo[i][2]) * (stoi(goodsInfo[i][CheckedDay + 1]) - stoi(goodsInfo[i][CheckedDay + 2]));
            saleSum += goodsSale;
            if (goodsInfo[i][0][0] == 'A') {
                snackSum += goodsSale;
            }
            if (goodsInfo[i][0][0] == 'B') {
                beverageSum += goodsSale;
            }
        }

        cout << "* " << day / 100 << "월 " << day % 100 << "일 매출" << endl;
        cout << "* 전체 매출 : " << saleSum << "원" << endl;
        cout << "* 과자류 매출 : " << snackSum << "원" << endl;
        cout << "* 음료류 매출 : " << beverageSum << "원" << endl;
    }

    typedef struct { // 날짜와 매출을 멤버로 갖는 구조체
        int day;
        int sale;
    } dailySale;
    
    void SalesTrend() {
        dailySale a[CheckedDay - 1]; // 일별 매출 저장 위한 배열

        cout << "* 매출 추이" << "(" << stoi(goodsInfo[0][4]) / 100 << "/" << stoi(goodsInfo[0][4]) % 100 << " ~ " << stoi(goodsInfo[0][CheckedDay + 2]) / 100 << "/" << stoi(goodsInfo[0][CheckedDay + 2]) % 100 << ")" << endl;

        for (int i = 3; i < CheckedDay + 2; i ++) {
            int sum = 0;
            for (int j = 1; j < goodsNum + 1; j ++) {
                sum += stoi(goodsInfo[j][2]) * (stoi(goodsInfo[j][i]) - stoi(goodsInfo[j][i + 1]));
            }
            a[i - 3].day = stoi(goodsInfo[0][i + 1]);
            a[i - 3].sale = sum; // 날짜 순으로 배열에 일 매출 저장
        }

        for (int i = 0; i < CheckedDay - 1; i ++) {
            cout << "* " << a[i].day / 100 << "/" << a[i].day % 100 << " : " << a[i].sale << "원" << endl;
        }

        DrawGrape(a);
        DescendingSale(a);
    }

    static bool compare(const dailySale& day1, const dailySale& day2) { // 구조체의 sale 멤버 내림차순 정렬
        return day1.sale >= day2.sale;
    }

    void DescendingSale(dailySale* a) {
        sort(a, a + CheckedDay - 1, compare);

        cout << endl <<  "* 매출 추이 (높은 순)" << endl;
        for (int i = 0; i < CheckedDay - 1; i ++) {
            cout << "* " << a[i].day / 100 << "/" << a[i].day % 100 << " : " << a[i].sale << "원" << endl;
        }
    }

    void DrawGrape(dailySale* a) {
        int max_bar = 0;
        cout << endl << "* 매출 추이 그래프" << endl;
        for (int i = 0; i < CheckedDay - 1; i ++) {
            if (a[i].sale / 1000 > max_bar) {
                max_bar = a[i].sale / 1000; // x축 길이 출력 위함
            }

            cout << a[i].day / 100 << "/" << a[i].day % 100 << " │";
            for (int j = 0; j < a[i].sale / 1000; j ++) {
                cout << "◼︎";
            }
            cout << endl;
        }

        cout << "      └";
        for (int i = 0; i < max_bar + 3; i ++) {
            cout << "─";
        }
        cout << endl;
    }
};

int main() {
    int FirstDay, FirstGoodsNum, CheckedDay;
    int iSelect, saleSelect;
    Goods daily; // main 함수 내에서 공용으로 사용을 위해 선언.
    string toMenu;

    do {
        cout << "┌─────────────────────────────┐\n";
        cout << "│ *편의점 재고 관리 프로그램* │\n";
        cout << "│ 1. 재고 파일 불러오기       │\n";
        cout << "│ 2. 1일 1회 재고 등록        │\n";
        cout << "│ 3. 매출 확인                │\n";
        cout << "│ 4. 종료                     │\n";
        cout << "└─────────────────────────────┘\n";
        cout << "* 메뉴 선택 >> ";
        rewind(stdin);
        cin >> iSelect;

        switch (iSelect) {
            case 1:
            cout << "* 등록일 입력(ex: 1109) >> ";
            rewind(stdin);
            cin >> FirstDay;
            cout << "* 물건 개수 입력 >> ";
            rewind(stdin);
            cin >> FirstGoodsNum;
            cout << "* 재고 파일에 등록된 일수 입력 >> ";
            rewind(stdin);
            cin >> CheckedDay;
            daily = Goods(FirstDay, CheckedDay, FirstGoodsNum);
            break;

            case 2:
            cout << "* 날짜 입력(ex: 1110) >> ";
            rewind(stdin);
            cin >> FirstDay;
            daily.DailyCheck(FirstDay);
            daily.CheckedDay++;
            break;

            case 3:
            cout << "┌─────────────────────┐\n";
            cout << "│ - 매출 확인         │\n";
            cout << "│ 1. 일 매출 (" << FirstDay / 100 << "/" << FirstDay % 100 << ")" << "  │\n";
            cout << "│ 2. 매출 추이        │\n";
            cout << "└─────────────────────┘\n";
            cout << "* 메뉴 선택 >> ";
            rewind(stdin);
            cin >> saleSelect;
            switch (saleSelect) {
                case 1:
                daily.TotalSales(FirstDay);
                cout << "* 메뉴로 돌아가려면 enter를 입력 >> ";
                rewind(stdin);
                getline(cin, toMenu);
                break;

                case 2:
                daily.SalesTrend();
                cout << "* 메뉴로 돌아가려면 enter를 입력 >> ";
                rewind(stdin);
                getline(cin, toMenu);
                break;

                default:
                cout << "* 올바른 메뉴를 선택하세요." << endl;
                break;
            }
            break;

            case 4:
            cout << "* 프로그램을 종료합니다." << endl;
            return 0;
            break;

            default:
            cout << "* 올바른 메뉴를 선택하세요." << endl;
            break;
        }
    } while (iSelect != 4);
}