#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;

//宏定义:CLEAR() 控制台清屏. 针对 windows 和 linux 调用不同的指令
#ifdef _WIN32
#define CLEAR() system("cls")
#else
#define CLEAR() system("clear")
#endif
//宏定义:OUT() 清屏,并输出上一步的操作结果所用到的宏
#define OUT(x)                                                      \
    CLEAR();                                                        \
    cout << "***************************************************\n" \
         << x

/* 程序第1部分, 数据结构声明: 分别为旅客/航班/订票 */
class Guest
{
  public:
    int id;       //客户编号,防止客户重名
    string name;  //客户名字
    string phone; //客户电话联系方式
    void load()
    {
        cout << "请输入客户名字(例如 Mike): ";
        cin >> name;
        cout << "请输入客户电话(例如 1388888888): ";
        cin >> phone;
    }
};
class Flight
{
  public:
    int id;            //航班 id
    string name;       //航班名字,例如 USA01
    string startPoint; //起点,例如 Beijing
    string endPoint;   //终点,例如 Beijing
    string price;      //价格,例如 $300
    string time;       //起飞时间,例如 2019/1/1-10:00
    void load()
    {
        cout << "请输入航班名字(例如  USA02): ";
        cin >> name;
        cout << "请输入航程起点(例如  Beijing): ";
        cin >> startPoint;
        cout << "请输入航程终点(例如  Beijing): ";
        cin >> endPoint;
        cout << "请输入航班价格(例如  $300): ";
        cin >> price;
        cout << "请输入航程起飞时间(例如 2019/1/1-10:00): ";
        cin >> time;
    }
};
class Ticket
{
  public:
    int id;       //订票 id
    int guestId;  //对应的顾客 id
    int flightId; //对应的航班 id
    string state; //订单状态,可选值:["正常","顾客取消","航班取消"],后二者分别在删除顾客信息/删除航班信息时触发,以保证系统完整性
    void load()
    {
        cout << "请输入订票的顾客 id(例如 0): ";
        cin >> guestId;
        Guest guest;
        while (!findGuest(&guest))
        {
            cout << "未查询到该顾客,请重新输入顾客 id: ";
            cin >> guestId;
        }
        cout << "请输入订票的航班 id(例如 0): ";
        cin >> flightId;
        Flight flight;
        while (!findFlight(&flight))
        {
            cout << "未查询到该航班,请重新输入航班 id: ";
            cin >> flightId;
        }
        state = "正常";
    }
    //查找对应的顾客信息,若成功则赋值给 guest 参数并返回1,查不到则返回0
    int findGuest(Guest *guest);
    //查找对应的航班信息,若成功则赋值给 flight 参数并返回1,查不到则返回0
    int findFlight(Flight *flight);
    //打印这条订票信息
    void print()
    {
        Guest guest;
        findGuest(&guest);
        Flight flight;
        findFlight(&flight);
        cout << "      " << id << " - " << guest.name << " - ";
        cout << flight.startPoint << " => " << flight.endPoint << "\n";
        cout << "          价格:" << flight.price << " - 时间:" << flight.time;
        cout << "\n          状态: " << state << "\n";
    }
};

/* 
 * 程序第2部分, 全局变量声明
 * 以及实现前面定义的 Ticket 成员函数(为什么在这里实现?因为如果在上面实现的话会提示找不到 Guests 和 * Flights. 不写行不行?不行,因为不能让莫须有的乘客登上莫须有的航班呀,必须做检查!) 
 */
int GuestId = 0, FlightId = 0, TicketId = 0; //自动增长的序列号, 用于区别不同的存入信息
vector<Guest> Guests;
vector<Flight> Flights;
vector<Ticket> Tickets;

int Ticket::findGuest(Guest *guest)
{
    for (int i = 0; i < Guests.size(); i++)
    {
        if (Guests[i].id == this->guestId)
        {
            (*guest) = Guests[i];
            return 1;
        }
    }
    return 0;
}
int Ticket::findFlight(Flight *flight)
{
    for (int i = 0; i < Flights.size(); i++)
    {
        if (Flights[i].id == this->flightId)
        {
            (*flight) = Flights[i];
            return 1;
        }
    }
    return 0;
}

/* 程序第3部分, 处理函数声明, 带 Menu 后缀的为显示对应菜单*/
void init(); //程序初始化时先填入部分旅客/航班/订单信息
void Exit();
void mainMenu();
void guestMenu();
void flightMenu();
void ticketMenu();

/* 程序第4部分, main函数逻辑*/
int main(int argc, const char *argv[])
{
    init();
    int choice;
    while (1)
    {
        /* 程序开始,输出选项提示 */
        CLEAR();
        mainMenu();
        cin >> choice;
        switch (choice)
        {
        case 0:
            Exit();
            break;
        case 1:
            OUT("您选择了:1 查看,增加,修改,删除 旅客信息\n");
            guestMenu();
            break;
        case 2:
            OUT("您选择了:2.查看,增加,修改,删除 航班信息\n");
            flightMenu();
            break;
        case 3:
            OUT("您选择了:3.查看,增加,修改,删除 订票信息\n");
            ticketMenu();
            break;
        default:
            cout << "输入错误,请重新输入:";
            break;
        }
    }
    return 0;
}
/* 程序第5部分, 功能函数实现 */

//程序初始时填充一些数据, 从文件(guest.txt, flights.txt, tickets.txt)中分别读入
void init()
{
    ifstream file;
    //init第1步 从文件中读取客户信息
    file.open("guests.txt");
    while (!file.eof())
    {
        // 读取到临时变量里
        Guest guest;
        file >> guest.id >> guest.name >> guest.phone;
        //如果读入文件中有个大 id,我们的 GuestId 也要相应增大,以免出现重复 id
        GuestId = GuestId > guest.id ? GuestId + 1 : guest.id + 1;
        // 如果读取成功，保存。否则，中断读取过程
        if (file)
            Guests.push_back(guest);
        else
            break;
    }
    file.close();
    //init第2步 从文件中读取航班信息
    file.open("flights.txt");
    while (!file.eof())
    {
        // 读取到临时变量里
        Flight flight;
        file >> flight.id >> flight.name >> flight.startPoint >> flight.endPoint >> flight.price >> flight.time;
        //如果读入文件中有个大 id,我们的 FlightId 也要相应增大,以免出现重复 id
        FlightId = FlightId > flight.id ? FlightId + 1 : flight.id + 1;
        // 如果读取成功，保存。否则，中断读取过程
        if (file)
            Flights.push_back(flight);
        else
            break;
    }
    file.close();
    //init第3步 从文件中读取订票信息
    file.open("tickets.txt");
    while (!file.eof())
    {
        // 读取到临时变量里
        Ticket ticket;
        file >> ticket.id >> ticket.guestId >> ticket.flightId >> ticket.state;
        //如果读入文件中有个大 id,我们的 TicketId 也要相应增大,以免出现重复 id
        TicketId = TicketId > ticket.id ? TicketId + 1 : ticket.id + 1;
        // 如果读取成功，保存。否则，中断读取过程
        if (file)
            Tickets.push_back(ticket);
        else
            break;
    }
    file.close();
}

//输出主菜单
void mainMenu()
{
    cout << "***************************************************\n";
    cout << "   欢迎使用 XX 航空服务管理系统,请输入数字来选择操作:\n";
    cout << "     1.查看,增加,修改,删除 旅客信息\n";
    cout << "     2.查看,增加,修改,删除 航班信息\n";
    cout << "     3.查看,增加,修改,删除 订票信息\n";
    cout << "     0.退出\n";
    cout << "***************************************************\n";
    cout << "你的选择是:";
}
//退出时将数据写回原文件
void Exit()
{
    ofstream file;
    file.open("guests.txt");
    for (int i = 0; i < Guests.size(); i++)
    {
        Guest g = Guests[i];
        file << g.id << " " << g.name << " " << g.phone << "\n";
    }
    file.close();
    file.open("flights.txt");
    for (int i = 0; i < Flights.size(); i++)
    {
        Flight g = Flights[i];
        file << g.id << " " << g.name << " " << g.startPoint << " ";
        file << g.endPoint << " " << g.price << " " << g.time << "\n";
    }
    file.close();
    cout << "感谢您的使用!再见!\n";
    exit(0);
}
//输出旅客信息管理菜单
void guestMenu()
{
    cout << "***************************************************\n";
    cout << "    旅客信息:\n";
    for (vector<Guest>::iterator i = Guests.begin(); i != Guests.end(); ++i)
    {
        cout << "      " << i->id << " - " << i->name << " - " << i->phone << endl;
    }
    cout << "---------------------------------------------------\n";
    cout << "    可选操作:\n";
    cout << "     1.增加一个旅客信息\n";
    cout << "     2.修改一个旅客信息\n";
    cout << "     3.删除一个旅客信息\n";
    cout << "     0.返回主菜单\n";
    cout << "***************************************************\n";
    cout << "你的选择是:";
    while (1)
    {

        int choice;
        cin >> choice;
        switch (choice)
        {
        case 0:
            return;
            break;
        case 1:
        {
            Guest guest;
            guest.load();
            guest.id = GuestId++;
            Guests.push_back(guest);
            OUT("添加成功!\n");
            guestMenu();
            return;
        }
        case 2:
        {
            Guest guest;
            cout << "请输入要修改的旅客id(例如 0):";
            cin >> guest.id;
            guest.load();
            for (int i = 0; i < Guests.size(); i++)
            {
                if (Guests[i].id == guest.id)
                {
                    Guests[i] = guest;
                    OUT("修改成功!\n");
                    guestMenu();
                    return;
                }
            }
            CLEAR();
            cout << "修改失败!未查询到此用户id\n";
            guestMenu();
            return;
        }
        case 3:
        {
            cout << "请输入要删除的旅客id(例如0): ";
            int id;
            cin >> id;
            for (vector<Guest>::iterator it = Guests.begin(); it != Guests.end();)
            {
                if ((*it).id == id)
                {
                    //删除顾客之前要把所有相关订单的状态改为"顾客取消"
                    for(int i = 0;i<Tickets.size();i++){
                        if(Tickets[i].guestId == it->id){
                            Tickets[i].state = "顾客取消";
                        }
                    }
                    it = Guests.erase(it);
                    OUT("删除成功!\n");
                    guestMenu();
                    return;
                }
                else
                {
                    it++;
                }
            }
            OUT("删除失败!未查询到此用户id\n");
            guestMenu();
            return;
        }
        default:
            cout << "输入错误,请重新输入:";
            break;
        }
    }
}

//输出航班信息管理菜单
void flightMenu()
{
    cout << "***************************************************\n";
    cout << "    航班信息:\n";
    for (vector<Flight>::iterator i = Flights.begin(); i != Flights.end(); ++i)
    {
        cout << "      " << i->id << " - " << i->name << " - " << i->startPoint << " => " << i->endPoint << "\n";
        cout << "          票价: " << i->price << "  起飞时间: " << i->time << "\n";
    }
    cout << "---------------------------------------------------\n";
    cout << "    可选操作:\n";
    cout << "     1.增加一个航班信息\n";
    cout << "     2.修改一个航班信息\n";
    cout << "     3.删除一个航班信息\n";
    cout << "     0.返回主菜单\n";
    cout << "***************************************************\n";
    cout << "你的选择是:";
    int choice;
    while (1)
    {
        cin >> choice;
        switch (choice)
        {
        case 0:
            return;
            break;
        case 1:
        {
            Flight flight;
            flight.load();
            flight.id = FlightId++;
            Flights.push_back(flight);
            OUT("添加成功!\n");
            flightMenu();
            return;
        }
        case 2:
        {
            Flight flight;
            cout << "请输入要修改的航班id: ";
            cin >> flight.id;
            flight.load();
            for (int i = 0; i < Flights.size(); i++)
            {
                if (Flights[i].id == flight.id)
                {
                    Flights[i] = flight;
                    OUT("修改成功!\n");
                    flightMenu();
                    return;
                }
            }
            OUT("修改失败!未查询到该航班 id\n");
            flightMenu();
            return;
        }
        case 3:
        {
            cout << "请输入要删除的航班id(例如0): ";
            int id;
            cin >> id;
            for (vector<Flight>::iterator it = Flights.begin(); it != Flights.end();)
            {
                if ((*it).id == id)
                {
                    //删除航班之前要把所有相关订单的状态改为"航班取消"
                    for(int i = 0;i<Tickets.size();i++){
                        if(Tickets[i].flightId == it->id){
                            Tickets[i].state = "航班取消";
                        }
                    }
                    it = Flights.erase(it);
                    OUT("删除成功!\n");
                    flightMenu();
                    return;
                }
                else
                    it++;
            }
            OUT("删除失败!未查询到该航班 id!\n");
            flightMenu();
            return;
        }
        default:
            cout << "输入错误,请重新输入:";
            break;
        }
    }
}

//输出订票信息管理菜单
void ticketMenu()
{
    cout << "***************************************************\n";
    cout << "    可选操作:\n";
    cout << "     1.增加一个订票信息\n";
    cout << "     2.修改一个订票信息\n";
    cout << "     3.删除一个订票信息\n";
    cout << "     4.查询全部订票信息\n";
    cout << "     5.查询订票信息-按旅客名\n";
    cout << "     6.查询订票信息-按航班名\n";
    cout << "     7.查询订票信息-按起点名\n";
    cout << "     8.查询订票信息-按终点名\n";
    cout << "     0.返回主菜单\n";
    cout << "***************************************************\n";
    cout << "你的选择是:";
    int choice;
    while (1)
    {
        cin >> choice;
        switch (choice)
        {
        case 0:
            return;
            break;
        case 1:
        {
            Ticket ticket;
            ticket.load();
            ticket.id = TicketId++;
            Tickets.push_back(ticket);
            OUT("添加成功!\n");
            ticketMenu();
            return;
        }
        case 2:
        {
            cout << "请输入要修改的订票id: ";
            Ticket ticket;
            ticket.load();
            for (int i = 0; i < Tickets.size(); i++)
            {
                if (Tickets[i].id == ticket.id)
                {
                    Tickets[i] = ticket;
                    OUT("修改成功!\n");
                    ticketMenu();
                    return;
                }
            }
            OUT("修改失败!未查询到该订票 id\n");
            ticketMenu();
            return;
        }
        case 3:
        {
            cout << "请输入要删除的订票id(例如0): ";
            int id;
            cin >> id;
            for (vector<Ticket>::iterator it = Tickets.begin(); it != Tickets.end();)
            {
                if ((*it).id == id)
                {
                    it = Tickets.erase(it);
                    OUT("删除成功!\n");
                    ticketMenu();
                    return;
                }
                else
                {
                    it++;
                }
            }
            OUT("删除失败!未查询到该订票 id\n");
            ticketMenu();
            return;
        }
        case 4:
        {
            OUT("订票信息:\n");
            for (vector<Ticket>::iterator i = Tickets.begin(); i != Tickets.end(); ++i)
            {
                i->print();
            }
            ticketMenu();
            return;
        }
        case 5:
        {
            cout << "请输入要指定查询的旅客名(例如 Mike):";
            string name;
            cin >> name;
            OUT("查询结果:\n");
            for (vector<Ticket>::iterator i = Tickets.begin(); i != Tickets.end(); ++i)
            {
                Guest guest;
                if (i->findGuest(&guest) && guest.name == name)
                {
                    i->print();
                }
            }
            ticketMenu();
            return;
        }
        case 6:
        {
            cout << "请输入要指定的航班名(例如  USA01):";
            string name;
            cin >> name;
            OUT("查询结果:\n");
            for (vector<Ticket>::iterator i = Tickets.begin(); i != Tickets.end(); ++i)
            {
                Flight flight;
                if (i->findFlight(&flight) && flight.name == name)
                {
                    i->print();
                }
            }
            ticketMenu();
            return;
        }
        case 7:
        {
            cout << "请输入要指定的起点(例如 Shanghai):";
            string name;
            cin >> name;
            OUT("查询结果:\n");
            for (vector<Ticket>::iterator i = Tickets.begin(); i != Tickets.end(); ++i)
            {
                Flight flight;
                if (i->findFlight(&flight) && flight.startPoint == name)
                {
                    i->print();
                }
            }
            ticketMenu();
            return;
        }
        case 8:
        {
            cout << "请输入要指定的终点(例如 Beijing):";
            string name;
            cin >> name;
            OUT("查询结果:\n");
            for (vector<Ticket>::iterator i = Tickets.begin(); i != Tickets.end(); ++i)
            {
                Flight flight;
                if (i->findFlight(&flight) && flight.endPoint == name)
                {
                    i->print();
                }
            }
            ticketMenu();
            return;
        }
        default:
            cout << "输入错误,请重新输入:";
            break;
        }
    }
}