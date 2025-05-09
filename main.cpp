#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <memory>
#include <algorithm>
#include <string>
#include <ctime>
using namespace std;

class Resource {
public:
    string title;
    string author;
    int resource_id;
    int year;
    bool isAvailable = true;

    virtual string getDetails() const {
        return title + " by " + author;
    }
    void markIssued() { isAvailable = false; }
    void markReturned() { isAvailable = true; }
    virtual ~Resource() {}
};

class Book : public Resource {
public:
    string publisher;
    string ISBN;
};

class EBook : public Resource {
public:
    string file_size;
    string format;
};

class ResearchJournal : public Resource {
public:
    string journal_name;
    int volume;
};

class Fine {
public:
    int days_late;
    static constexpr int rate_per_day = 5;

    Fine(int days) : days_late(days) {}

    int calculateFine() const {
        return days_late * rate_per_day;
    }
};

class Transaction {
public:
    int resource_id;
    int member_id;
    time_t date;
    string action; // "issue" or "return"
    int fine_amount = 0;

    string getSummary() const {
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&date));
        return "Action: " + action + ", Resource ID: " + to_string(resource_id) + ", Member ID: " + to_string(member_id) + ", Date: " + buffer + ", Fine: Rs. " + to_string(fine_amount);
    }
};

class Member {
public:
    string name;
    int member_id;
    string email;
    string phone;
    stack<shared_ptr<Resource>> issued_resources;
    int total_fine = 0;

    void issueResource(shared_ptr<Resource> r) {
        issued_resources.push(r);
        r->markIssued();
    }

    void returnResource(shared_ptr<Resource> r, int days_late) {
        if (!issued_resources.empty()) issued_resources.pop();
        r->markReturned();
        Fine fine(days_late);
        total_fine += fine.calculateFine();
    }

    vector<shared_ptr<Resource>> viewIssued() const {
        vector<shared_ptr<Resource>> list;
        stack<shared_ptr<Resource>> temp = issued_resources;
        while (!temp.empty()) {
            list.push_back(temp.top());
            temp.pop();
        }
        return list;
    }

    int calculateTotalFine() const {
        return total_fine;
    }
};

class Library {
public:
    vector<shared_ptr<Resource>> resources;
    list<shared_ptr<Member>> members;
    queue<Transaction> transactions;
    stack<Transaction> recentActivity;

    void addResource(shared_ptr<Resource> r) {
        resources.push_back(r);
    }

    void removeResource(int id) {
        resources.erase(remove_if(resources.begin(), resources.end(), [id](shared_ptr<Resource> r) {
            return r->resource_id == id;
        }), resources.end());
    }

    void addMember(shared_ptr<Member> m) {
        members.push_back(m);
    }

    void removeMember(int id) {
        members.remove_if([id](shared_ptr<Member> m) {
            return m->member_id == id;
        });
    }

    shared_ptr<Member> findMember(int id) {
        for (auto& m : members)
            if (m->member_id == id) return m;
        return nullptr;
    }

    shared_ptr<Resource> findResource(int id) {
        for (auto& r : resources)
            if (r->resource_id == id) return r;
        return nullptr;
    }

    string generateReport() {
        string report = "--- Library Report ---\n";
        report += "Total Resources: " + to_string(resources.size()) + "\n";
        report += "Total Members: " + to_string(members.size()) + "\n";
        report += "Total Transactions: " + to_string(transactions.size()) + "\n";
        return report;
    }
};

template <typename T>
class SearchSorter {
public:
    vector<shared_ptr<T>> sortByTitle(const vector<shared_ptr<T>>& list) {
        vector<shared_ptr<T>> sorted = list;
        sort(sorted.begin(), sorted.end(), [](auto a, auto b) {
            return a->title < b->title;
        });
        return sorted;
    }

    vector<shared_ptr<T>> sortByID(const vector<shared_ptr<T>>& list) {
        vector<shared_ptr<T>> sorted = list;
        sort(sorted.begin(), sorted.end(), [](auto a, auto b) {
            return a->resource_id < b->resource_id;
        });
        return sorted;
    }

    shared_ptr<T> searchByTitle(const vector<shared_ptr<T>>& list, const string& title) {
        for (auto& item : list)
            if (item->title == title) return item;
        return nullptr;
    }

    shared_ptr<T> searchByID(const vector<shared_ptr<T>>& list, int id) {
        for (auto& item : list)
            if (item->resource_id == id) return item;
        return nullptr;
    }
};

int main() {
    // Sample usage
    Library lib;
    auto book = make_shared<Book>();
    book->title = "C++ Programming";
    book->author = "Bjarne Stroustrup";
    book->resource_id = 1;
    book->year = 2020;

    auto member = make_shared<Member>();
    member->name = "John Doe";
    member->member_id = 101;
    member->email = "john@example.com";
    member->phone = "1234567890";

    lib.addResource(book);
    lib.addMember(member);

    member->issueResource(book);
    Transaction t{1, 101, time(0), "issue", 0};
    lib.transactions.push(t);
    lib.recentActivity.push(t);

    cout << lib.generateReport();
    return 0;
}
