#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
using namespace std;
using std::string;
struct Contact
{
    string name, phone, email, address, notes;
    Contact(string n = "", string p = "", string e = "", string a = "", string nt = "")
        : name(n), phone(p), email(e), address(a), notes(nt) {}
};
const string DATA_FILE = "contacts.txt";
const char FIELD_SEPARATOR = '|';
const char RECORD_SEPARATOR = '\n';

string toLower(string s)
{
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

void clearBuffer()
{
    cin.clear();
    cin.ignore(1000, '\n');
}
void displayMenu();
void addContact(vector<Contact> &);
void viewContacts(const vector<Contact> &);
void searchContacts(const vector<Contact> &);
void deleteContact(vector<Contact> &);
void editContact(vector<Contact> &);
void showContact(const Contact &, int index = -1);
void saveContactsToFile(const vector<Contact> &contacts);
bool loadContactsFromFile(vector<Contact> &contacts);
string escapeString(const string &str);
string unescapeString(const string &str);

int main()
{
    vector<Contact> contacts;

    cout << "\t=======DIGITAL ADDRESS BOOK SYSTEM======\n";
    if (loadContactsFromFile(contacts))
    {
        cout << "\tLoaded " << contacts.size() << " contacts from file.\n";
    }
    else
    {
        cout << "\tNo existing contacts found or error reading file.\n";
        cout << "\tStarting with empty address book.\n";
    }

    while (true)
    {
        displayMenu();
        cout << "\tChoice: ";

        int choice;
        if (!(cin >> choice))
        {
            clearBuffer();
            continue;
        }
        clearBuffer();

        switch (choice)
        {
        case 1:
            addContact(contacts);
            saveContactsToFile(contacts); // Save after adding
            break;
        case 2:
            viewContacts(contacts);
            break;
        case 3:
            searchContacts(contacts);
            break;
        case 4:
            deleteContact(contacts);
            saveContactsToFile(contacts); // Save after deleting
            break;
        case 5:
            editContact(contacts);
            saveContactsToFile(contacts); // Save after editing
            break;
        case 6:
            saveContactsToFile(contacts);
            cout << "\tGoodbye! Contacts saved to file.\n";
            return 0;
        default:
            cout << "\tInvalid choice!\n";
        }
        cout << "\n\tPress Enter...";
        cin.get();
    }
}

void displayMenu()
{
    cout << "\n\t========================================\n";
    cout << "\t              MAIN MENU\n";
    cout << "\t========================================\n";
    cout << "\t1. Add New Contact\n";
    cout << "\t2. View All Contacts\n";
    cout << "\t3. Search Contacts\n";
    cout << "\t4. Delete Contact\n";
    cout << "\t5. Edit Contact\n";
    cout << "\t6. Exit Program (Save & Exit)\n";
    cout << "\t========================================\n";
}

void addContact(vector<Contact> &contacts)
{
    Contact c;
    cout << "\n\t--- Add Contact ---\n";

    cout << "\tName: ";
    getline(cin, c.name);
    for (auto &contact : contacts)
        if (toLower(contact.name) == toLower(c.name))
        {
            cout << "\tDuplicate name! Continue? (y/n): ";
            if (cin.get() != 'y')
            {
                clearBuffer();
                return;
            }
            clearBuffer();
            break;
        }

    cout << "\tPhone: ";
    getline(cin, c.phone);
    cout << "\tEmail: ";
    getline(cin, c.email);
    cout << "\tAddress: ";
    getline(cin, c.address);
    cout << "\tNotes: ";
    getline(cin, c.notes);

    contacts.push_back(c);
    cout << "\tContact added!\n";
}

void viewContacts(const vector<Contact> &contacts)
{
    if (contacts.empty())
    {
        cout << "\tNo contacts.\n";
        return;
    }

    vector<Contact> sorted = contacts;
    sort(sorted.begin(), sorted.end(),
         [](const Contact &a, const Contact &b)
         { return toLower(a.name) < toLower(b.name); });

    cout << "\n\t--- All Contacts (" << sorted.size() << ") ---\n";
    for (size_t i = 0; i < sorted.size(); i++)
    {
        showContact(sorted[i], i + 1);
        if (i < sorted.size() - 1)
            cout << "\t----------------------------\n";
    }
}

void searchContacts(const vector<Contact> &contacts)
{
    if (contacts.empty())
    {
        cout << "\tNo contacts to search.\n";
        return;
    }

    cout << "\n\tSearch by: 1) Name 2) Phone 3) Email\nChoice: ";
    int opt;
    cin >> opt;
    clearBuffer();
    if (opt < 1 || opt > 3)
    {
        cout << "\tInvalid.\n";
        return;
    }

    cout << "\tSearch term: ";
    string term;
    getline(cin, term);
    string lowerTerm = toLower(term);

    vector<Contact> results;
    for (auto &c : contacts)
    {
        bool found = false;
        switch (opt)
        {
        case 1:
            if (toLower(c.name).find(lowerTerm) != string::npos)
                found = true;
            break;
        case 2:
            if (c.phone.find(term) != string::npos)
                found = true;
            break;
        case 3:
            if (toLower(c.email).find(lowerTerm) != string::npos)
                found = true;
            break;
        }
        if (found)
            results.push_back(c);
    }

    cout << "\n\t--- Found " << results.size() << " contact(s) ---\n";
    for (size_t i = 0; i < results.size(); i++)
    {
        showContact(results[i], i + 1);
        if (i < results.size() - 1)
            cout << "\t----------------------------\n";
    }
}

void deleteContact(vector<Contact> &contacts)
{
    if (contacts.empty())
    {
        cout << "\tNo contacts to delete.\n";
        return;
    }

    viewContacts(contacts);
    cout << "\n\tEnter contact number to delete (0 to cancel): ";
    int idx;
    cin >> idx;
    clearBuffer();

    if (idx == 0)
    {
        cout << "\tCancelled.\n";
        return;
    }
    if (idx < 1 || idx > (int)contacts.size())
    {
        cout << "\tInvalid number.\n";
        return;
    }

    cout << "\tDelete '" << contacts[idx - 1].name << "'? (y/n): ";
    if (cin.get() == 'y')
    {
        contacts.erase(contacts.begin() + idx - 1);
        cout << "\tDeleted!\n";
    }
    else
    {
        cout << "\tCancelled.\n";
    }
    clearBuffer();
}

void editContact(vector<Contact> &contacts)
{
    if (contacts.empty())
    {
        cout << "\tNo contacts to edit.\n";
        return;
    }

    viewContacts(contacts);
    cout << "\n\tEnter contact number to edit (0 to cancel): ";
    int idx;
    cin >> idx;
    clearBuffer();

    if (idx == 0)
    {
        cout << "\tCancelled.\n";
        return;
    }
    if (idx < 1 || idx > (int)contacts.size())
    {
        cout << "\tInvalid number.\n";
        return;
    }

    Contact &c = contacts[idx - 1];
    cout << "\n\t--- Editing: " << c.name << " ---\n";
    showContact(c);

    cout << "\n\tLeave blank to keep current value.\n";
    string temp;

    cout << "\tNew name: ";
    getline(cin, temp);
    if (!temp.empty())
        c.name = temp;

    cout << "\tNew phone: ";
    getline(cin, temp);
    if (!temp.empty())
        c.phone = temp;

    cout << "\tNew email: ";
    getline(cin, temp);
    if (!temp.empty())
        c.email = temp;

    cout << "\tNew address: ";
    getline(cin, temp);
    if (!temp.empty())
        c.address = temp;

    cout << "\tNew notes: ";
    getline(cin, temp);
    if (!temp.empty())
        c.notes = temp;

    cout << "\tUpdated!\n";
}

void showContact(const Contact &c, int index)
{
    if (index > 0)
        cout << "\tContact #" << index << ":\n";
    cout << left << setw(10) << "\tName:" << c.name << endl
         << setw(10) << "\tPhone:" << c.phone << endl
         << setw(10) << "\tEmail:" << c.email << endl;
    if (!c.address.empty())
        cout << setw(10) << "\tAddress:" << c.address << endl;
    if (!c.notes.empty())
        cout << setw(10) << "\tNotes:" << c.notes << endl;
}

// Escape special characters in strings for file storage
string escapeString(const string &str)
{
    string escaped;
    for (char ch : str)
    {
        if (ch == FIELD_SEPARATOR)
        {
            escaped += "\\f";
        }
        else if (ch == '\\')
        {
            escaped += "\\\\";
        }
        else if (ch == '\n')
        {
            escaped += "\\n";
        }
        else if (ch == '\r')
        {
            escaped += "\\r";
        }
        else
        {
            escaped += ch;
        }
    }
    return escaped;
}

// Unescape strings when reading from file
string unescapeString(const string &str)
{
    string unescaped;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == '\\' && i + 1 < str.length())
        {
            if (str[i + 1] == 'f')
            {
                unescaped += FIELD_SEPARATOR;
                i++;
            }
            else if (str[i + 1] == '\\')
            {
                unescaped += '\\';
                i++;
            }
            else if (str[i + 1] == 'n')
            {
                unescaped += '\n';
                i++;
            }
            else if (str[i + 1] == 'r')
            {
                unescaped += '\r';
                i++;
            }
            else
            {
                unescaped += str[i];
            }
        }
        else
        {
            unescaped += str[i];
        }
    }
    return unescaped;
}

void saveContactsToFile(const vector<Contact> &contacts)
{
    ofstream outFile(DATA_FILE);

    if (!outFile)
    {
        cerr << "\tError: Could not open file for writing: " << DATA_FILE << endl;
        return;
    }
    for (const auto &contact : contacts)
    {
        outFile << escapeString(contact.name) << FIELD_SEPARATOR
                << escapeString(contact.phone) << FIELD_SEPARATOR
                << escapeString(contact.email) << FIELD_SEPARATOR
                << escapeString(contact.address) << FIELD_SEPARATOR
                << escapeString(contact.notes) << RECORD_SEPARATOR;
    }
    outFile.close();
}

// Load contacts from file
bool loadContactsFromFile(vector<Contact> &contacts)
{
    ifstream inFile(DATA_FILE);

    if (!inFile)
    {
        // File doesn't exist yet, which is normal for first run
        return false;
    }

    contacts.clear(); // Clear existing contacts

    string line;
    while (getline(inFile, line))
    {
        if (line.empty())continue; // Skip empty lines
        vector<string> fields;
        stringstream ss(line);
        string field;
        // Split line into fields using separator
        while (getline(ss, field, FIELD_SEPARATOR))
        {
            fields.push_back(field);
        }
        if (fields.size() >= 5)
        {
            Contact c;
            c.name = unescapeString(fields[0]);
            c.phone = unescapeString(fields[1]);
            c.email = unescapeString(fields[2]);
            c.address = unescapeString(fields[3]);
            c.notes = unescapeString(fields[4]);

            contacts.push_back(c);
        }
    }

    inFile.close();
    return true;
}