#include "Array.h"
#include "SinglyList.h"
#include "Commands.h"
#include "DoublyList.h"
#include "Stack.h"
#include "Queue.h"
#include "Tree.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <algorithm>
#include <vector>
#include <map>

using namespace std;

// ----- Структура для хранения всех структур данных -----
struct Database {
    map<string, Array> arrays;
    map<string, SingleList> singlyLists;
    map<string, DoublyList> doublyLists;
    map<string, Stack> stacks;
    map<string, Queue> queues;
    map<string, Tree> trees;
};

// ----- Вспомогательные функции для загрузки и сохранения -----
void loadDatabaseFromFile(const string& filename, Database& db) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        return; // Файл не существует, база пустая
    }

    string line;
    string currentStructure;
    string currentType;

    while (getline(fin, line)) {
        if (line.empty()) continue;

        // Проверяем, является ли строка заголовком структуры
        if (line[0] == '[' && line[line.length()-1] == ']') {
            string header = line.substr(1, line.length()-2);
            size_t pos = header.find(':');
            if (pos != string::npos) {
                currentStructure = header.substr(0, pos);
                currentType = header.substr(pos + 1);
            }
            continue;
        }

        // Загружаем данные в соответствующую структуру
        if (currentType == "ARRAY") {
            MPUSH(db.arrays[currentStructure], line);
        }
        else if (currentType == "SINGLY_LIST") {
            FADD_TAIL(db.singlyLists[currentStructure], line);
        }
        else if (currentType == "DOUBLY_LIST") {
            LADD_TAIL(db.doublyLists[currentStructure], line);
        }
        else if (currentType == "STACK") {
            SPUSH(db.stacks[currentStructure], line);
        }
        else if (currentType == "QUEUE") {
            QPUSH(db.queues[currentStructure], line);
        }
        else if (currentType == "TREE") {
            TADD(db.trees[currentStructure], line, true);
        }
    }
    fin.close();
}

void saveDatabaseToFile(const string& filename, Database& db) {
    ofstream fout(filename);
    if (!fout.is_open()) return;

    // Сохраняем массивы
    for (auto& pair : db.arrays) {
        fout << "[" << pair.first << ":ARRAY]" << endl;
        Array& arr = pair.second;
        for (uint64_t i = 0; i < arr.size; i++) {
            fout << arr.data[i] << endl;
        }
        fout << endl;
    }

    // Сохраняем односвязные списки
    for (auto& pair : db.singlyLists) {
        fout << "[" << pair.first << ":SINGLY_LIST]" << endl;
        SingleList& list = pair.second;
        FNode* curr = list.head;
        while (curr) {
            fout << curr->data << endl;
            curr = curr->next;
        }
        fout << endl;
    }

    // Сохраняем двусвязные списки
    for (auto& pair : db.doublyLists) {
        fout << "[" << pair.first << ":DOUBLY_LIST]" << endl;
        DoublyList& list = pair.second;
        LNode* curr = list.head;
        while (curr) {
            fout << curr->data << endl;
            curr = curr->next;
        }
        fout << endl;
    }

    // Сохраняем стеки (в обратном порядке)
    for (auto& pair : db.stacks) {
        fout << "[" << pair.first << ":STACK]" << endl;
        Stack& stack = pair.second;
        vector<string> temp;
        SNode* curr = stack.top;
        while (curr) {
            temp.push_back(curr->data);
            curr = curr->next;
        }
        for (int i = temp.size() - 1; i >= 0; i--) {
            fout << temp[i] << endl;
        }
        fout << endl;
    }

    // Сохраняем очереди
    for (auto& pair : db.queues) {
        fout << "[" << pair.first << ":QUEUE]" << endl;
        Queue& queue = pair.second;
        QNode* curr = queue.front;
        while (curr) {
            fout << curr->data << endl;
            curr = curr->next;
        }
        fout << endl;
    }

    // Сохраняем деревья (level-order)
    for (auto& pair : db.trees) {
        fout << "[" << pair.first << ":TREE]" << endl;
        Tree& tree = pair.second;
        if (tree.root) {
            queue<TNode*> q;
            q.push(tree.root);
            while (!q.empty()) {
                TNode* current = q.front();
                q.pop();
                fout << current->data << endl;
                if (current->left) q.push(current->left);
                if (current->right) q.push(current->right);
            }
        }
        fout << endl;
    }

    fout.close();
}

// ----- Универсальная функция PRINT -----
void PRINT(const Database& db, const string& structureName, const string& structureType = "") {
    string type = structureType;
    transform(type.begin(), type.end(), type.begin(), ::toupper);

    if (type == "ARRAY" || type == "M") {
        auto it = db.arrays.find(structureName);
        if (it != db.arrays.end()) {
            cout << "ARRAY '" << structureName << "': ";
            MPRINT(it->second);
        } else {
            cout << "Массив '" << structureName << "' не найден!" << endl;
        }
    }
    else if (type == "SINGLY_LIST" || type == "SINGLY" || type == "F") {
        auto it = db.singlyLists.find(structureName);
        if (it != db.singlyLists.end()) {
            cout << "SINGLY LIST '" << structureName << "': ";
            FPRINT(it->second);
        } else {
            cout << "Односвязный список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (type == "DOUBLY_LIST" || type == "DOUBLY" || type == "L") {
        auto it = db.doublyLists.find(structureName);
        if (it != db.doublyLists.end()) {
            cout << "DOUBLY LIST '" << structureName << "': ";
            LPRINT(it->second);
        } else {
            cout << "Двусвязный список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (type == "STACK" || type == "S") {
        auto it = db.stacks.find(structureName);
        if (it != db.stacks.end()) {
            cout << "STACK '" << structureName << "': ";
            SPRINT(it->second);
        } else {
            cout << "Стек '" << structureName << "' не найден!" << endl;
        }
    }
    else if (type == "QUEUE" || type == "Q") {
        auto it = db.queues.find(structureName);
        if (it != db.queues.end()) {
            cout << "QUEUE '" << structureName << "': ";
            QPRINT(it->second);
        } else {
            cout << "Очередь '" << structureName << "' не найден!" << endl;
        }
    }
    else if (type == "TREE" || type == "T") {
        auto it = db.trees.find(structureName);
        if (it != db.trees.end()) {
            cout << "TREE '" << structureName << "': ";
            TLEVELORDER(it->second);
        } else {
            cout << "Дерево '" << structureName << "' не найден!" << endl;
        }
    }
    else {
        cout << "Неизвестный тип структуры: " << structureType << endl;
        cout << "Доступные: ARRAY(M), SINGLY_LIST(F), DOUBLY_LIST(L), STACK(S), QUEUE(Q), TREE(T)" << endl;
    }
}

// ----- Обработка команд -----
void Commands::execute(const string& query, string& filename) {
    istringstream iss(query);
    string command;
    iss >> command;

    // Загружаем базу данных
    Database db;
    loadDatabaseFromFile(filename, db);

    // Универсальная команда PRINT
    if (command == "PRINT") {
        string structureName, structureType;
        iss >> structureName >> structureType;
        PRINT(db, structureName, structureType);
        return;
    }

    // Команда LIST для просмотра всех структур
    if (command == "LIST") {
        cout << "Структуры в базе данных '" << filename << "':" << endl;
        
        if (!db.arrays.empty()) {
            cout << "Массивы: ";
            for (auto& pair : db.arrays) {
                cout << pair.first << " ";
            }
            cout << endl;
        }
        
        if (!db.singlyLists.empty()) {
            cout << "Односвязные списки: ";
            for (auto& pair : db.singlyLists) {
                cout << pair.first << " ";
            }
            cout << endl;
        }
        
        if (!db.doublyLists.empty()) {
            cout << "Двусвязные списки: ";
            for (auto& pair : db.doublyLists) {
                cout << pair.first << " ";
            }
            cout << endl;
        }
        
        if (!db.stacks.empty()) {
            cout << "Стеки: ";
            for (auto& pair : db.stacks) {
                cout << pair.first << " ";
            }
            cout << endl;
        }
        
        if (!db.queues.empty()) {
            cout << "Очереди: ";
            for (auto& pair : db.queues) {
                cout << pair.first << " ";
            }
            cout << endl;
        }
        
        if (!db.trees.empty()) {
            cout << "Деревья: ";
            for (auto& pair : db.trees) {
                cout << pair.first << " ";
            }
            cout << endl;
        }
        
        saveDatabaseToFile(filename, db);
        return;
    }

    // Команда для смены файла
    if (command == "CHANGE_FILE") {
        string newFilename;
        iss >> newFilename;
        if (!newFilename.empty()) {
            filename = newFilename;
            cout << "-> Рабочий файл изменен на: " << filename << endl;
            return;
        } else {
            cout << "Ошибка: укажите имя файла" << endl;
            return;
        }
    }

    string structureName;
    iss >> structureName;

    // ---- TREE ----
    if (command == "TADD") {
        string value; iss >> value;
        TADD(db.trees[structureName], value);
        saveDatabaseToFile(filename, db);
        cout << "-> вставлен " << value << " в дерево '" << structureName << "'" << endl;
    }
    else if (command == "TFIND") {
        string value; iss >> value;
        auto it = db.trees.find(structureName);
        if (it != db.trees.end()) {
            TFIND(it->second, value);
        } else {
            cout << "Дерево '" << structureName << "' не найдено!" << endl;
        }
    }
    else if (command == "TISFULL") {
        auto it = db.trees.find(structureName);
        if (it != db.trees.end()) {
            bool ok = TISFULL(it->second);
            cout << "-> " << (ok ? "FULL" : "NOT FULL") << endl;
        } else {
            cout << "Дерево '" << structureName << "' не найдено!" << endl;
        }
    }
    else if (command == "TLEVELORDER") {
        auto it = db.trees.find(structureName);
        if (it != db.trees.end()) {
            TLEVELORDER(it->second);
        } else {
            cout << "Дерево '" << structureName << "' не найдено!" << endl;
        }
    }
    else if (command == "TPRINT") {
        auto it = db.trees.find(structureName);
        if (it != db.trees.end()) {
            TPRINT_STRUCTURE(it->second);
        } else {
            cout << "Дерево '" << structureName << "' не найдено!" << endl;
        }
    }
    else if (command == "TPREORDER") {
        auto it = db.trees.find(structureName);
        if (it != db.trees.end()) {
            TPREORDER(it->second);
        } else {
            cout << "Дерево '" << structureName << "' не найдено!" << endl;
        }
    }
    else if (command == "TINORDER") {
        auto it = db.trees.find(structureName);
        if (it != db.trees.end()) {
            TINORDER(it->second);
        } else {
            cout << "Дерево '" << structureName << "' не найдено!" << endl;
        }
    }
    else if (command == "TPOSTORDER") {
        auto it = db.trees.find(structureName);
        if (it != db.trees.end()) {
            TPOSTORDER(it->second);
        } else {
            cout << "Дерево '" << structureName << "' не найдено!" << endl;
        }
    }
    else if (command == "TCLEAR") {
        auto it = db.trees.find(structureName);
        if (it != db.trees.end()) {
            TCLEAR(it->second);
            saveDatabaseToFile(filename, db);
            cout << "-> дерево '" << structureName << "' очищено" << endl;
        } else {
            cout << "Дерево '" << structureName << "' не найдено!" << endl;
        }
    }

    // ---- ARRAY ----
    else if (command == "MPUSH") {
        string value; iss >> value;
        MPUSH(db.arrays[structureName], value);
        saveDatabaseToFile(filename, db);
        cout << "-> добавлен " << value << " в массив '" << structureName << "'" << endl;
    }
    else if (command == "MADD_INDEX") {
        string value; uint64_t index;
        iss >> value >> index;
        auto it = db.arrays.find(structureName);
        if (it != db.arrays.end()) {
            MADD_INDEX(it->second, value, index);
            saveDatabaseToFile(filename, db);
            cout << "-> вставлен " << value << " по индексу " << index << " в массив '" << structureName << "'" << endl;
        } else {
            cout << "Массив '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "MREMOVE") {
        uint64_t index; iss >> index;
        auto it = db.arrays.find(structureName);
        if (it != db.arrays.end()) {
            if (index < it->second.size) {
                MREMOVE(it->second, index);
                saveDatabaseToFile(filename, db);
                cout << "-> удалён элемент по индексу " << index << " из массива '" << structureName << "'" << endl;
            } else {
                cout << "Ошибка: индекс " << index << " вне диапазона" << endl;
            }
        } else {
            cout << "Массив '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "MREPLACE") {
        uint64_t index; string value;
        iss >> index >> value;
        auto it = db.arrays.find(structureName);
        if (it != db.arrays.end()) {
            MREPLACE(it->second, index, value);
            saveDatabaseToFile(filename, db);
            cout << "-> заменён элемент по индексу " << index << " на " << value << " в массиве '" << structureName << "'" << endl;
        } else {
            cout << "Массив '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "MGET") {
        uint64_t index; iss >> index;
        auto it = db.arrays.find(structureName);
        if (it != db.arrays.end()) {
            try {
                string result = MGET(it->second, index);
                cout << "-> " << result << endl;
            } catch (const out_of_range& e) {
                cout << "Ошибка: " << e.what() << endl;
            }
        } else {
            cout << "Массив '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "MLENGTH") {
        auto it = db.arrays.find(structureName);
        if (it != db.arrays.end()) {
            uint64_t length = MLENGTH(it->second);
            cout << "-> " << length << endl;
        } else {
            cout << "Массив '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "MPRINT") {
        auto it = db.arrays.find(structureName);
        if (it != db.arrays.end()) {
            cout << "-> ";
            MPRINT(it->second);
        } else {
            cout << "Массив '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "MCLEAR") {
        auto it = db.arrays.find(structureName);
        if (it != db.arrays.end()) {
            MCLEAR(it->second);
            db.arrays.erase(it); // Удаляем из базы
            saveDatabaseToFile(filename, db);
            cout << "-> массив '" << structureName << "' очищен" << endl;
        } else {
            cout << "Массив '" << structureName << "' не найден!" << endl;
        }
    }

    // ---- SINGLY LINKED LIST ----
    else if (command == "FADD_HEAD") {
        string value; iss >> value;
        FADD_HEAD(db.singlyLists[structureName], value);
        saveDatabaseToFile(filename, db);
        cout << "-> добавлен " << value << " в голову списка '" << structureName << "'" << endl;
    }
    else if (command == "FADD_TAIL") {
        string value; iss >> value;
        FADD_TAIL(db.singlyLists[structureName], value);
        saveDatabaseToFile(filename, db);
        cout << "-> добавлен " << value << " в список '" << structureName << "'" << endl;
    }
    else if (command == "FADD_BEFORE") {
        string value, target; iss >> value >> target;
        auto it = db.singlyLists.find(structureName);
        if (it != db.singlyLists.end()) {
            FADD_BEFORE(it->second, value, target);
            saveDatabaseToFile(filename, db);
            cout << "-> вставлен " << value << " перед " << target << " в список '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "FADD_AFTER") {
        string value, target; iss >> value >> target;
        auto it = db.singlyLists.find(structureName);
        if (it != db.singlyLists.end()) {
            FADD_AFTER(it->second, value, target);
            saveDatabaseToFile(filename, db);
            cout << "-> вставлен " << value << " после " << target << " в список '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "FREMOVE_HEAD") {
        auto it = db.singlyLists.find(structureName);
        if (it != db.singlyLists.end()) {
            FREMOVE_HEAD(it->second);
            saveDatabaseToFile(filename, db);
            cout << "-> удалён первый элемент из списка '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "FREMOVE_TAIL") {
        auto it = db.singlyLists.find(structureName);
        if (it != db.singlyLists.end()) {
            FREMOVE_TAIL(it->second);
            saveDatabaseToFile(filename, db);
            cout << "-> удалён последний элемент из списка '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "FREMOVE_BEFORE") {
        string target; iss >> target;
        auto it = db.singlyLists.find(structureName);
        if (it != db.singlyLists.end()) {
            FREMOVE_BEFORE(it->second, target);
            saveDatabaseToFile(filename, db);
            cout << "-> удалён элемент перед " << target << " из списка '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "FREMOVE_AFTER") {
        string target; iss >> target;
        auto it = db.singlyLists.find(structureName);
        if (it != db.singlyLists.end()) {
            FREMOVE_AFTER(it->second, target);
            saveDatabaseToFile(filename, db);
            cout << "-> удалён элемент после " << target << " из списка '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "FREMOVE_BY_VALUE") {
        string value; iss >> value;
        auto it = db.singlyLists.find(structureName);
        if (it != db.singlyLists.end()) {
            bool ok = FREMOVE_BY_VALUE(it->second, value);
            saveDatabaseToFile(filename, db);
            if (ok)
                cout << "-> удалён " << value << " из списка '" << structureName << "'" << endl;
            else
                cout << "-> элемент " << value << " не найден в списке '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "FFIND") {
        string value; iss >> value;
        auto it = db.singlyLists.find(structureName);
        if (it != db.singlyLists.end()) {
            FFIND(it->second, value);
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "FPRINT") {
        auto it = db.singlyLists.find(structureName);
        if (it != db.singlyLists.end()) {
            cout << "-> ";
            FPRINT(it->second);
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "FPRINT_FORWARD") {
        auto it = db.singlyLists.find(structureName);
        if (it != db.singlyLists.end()) {
            cout << "-> ";
            FPRINT_FORWARD(it->second);
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "FPRINT_BACKWARD") {
        auto it = db.singlyLists.find(structureName);
        if (it != db.singlyLists.end()) {
            cout << "-> ";
            FPRINT_BACKWARD(it->second);
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "FCLEAR") {
        auto it = db.singlyLists.find(structureName);
        if (it != db.singlyLists.end()) {
            FCLEAR(it->second);
            db.singlyLists.erase(it);
            saveDatabaseToFile(filename, db);
            cout << "-> список '" << structureName << "' очищен" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }

    // ---- DOUBLY LINKED LIST ----
    else if (command == "LADD_HEAD") {
        string value; iss >> value;
        LADD_HEAD(db.doublyLists[structureName], value);
        saveDatabaseToFile(filename, db);
        cout << "-> добавлен " << value << " в голову списка '" << structureName << "'" << endl;
    }
    else if (command == "LADD_TAIL") {
        string value; iss >> value;
        LADD_TAIL(db.doublyLists[structureName], value);
        saveDatabaseToFile(filename, db);
        cout << "-> добавлен " << value << " в список '" << structureName << "'" << endl;
    }
    else if (command == "LADD_BEFORE") {
        string value, target; iss >> value >> target;
        auto it = db.doublyLists.find(structureName);
        if (it != db.doublyLists.end()) {
            LADD_BEFORE(it->second, value, target);
            saveDatabaseToFile(filename, db);
            cout << "-> вставлен " << value << " перед " << target << " в список '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "LADD_AFTER") {
        string value, target; iss >> value >> target;
        auto it = db.doublyLists.find(structureName);
        if (it != db.doublyLists.end()) {
            LADD_AFTER(it->second, value, target);
            saveDatabaseToFile(filename, db);
            cout << "-> вставлен " << value << " после " << target << " в список '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "LREMOVE_HEAD") {
        auto it = db.doublyLists.find(structureName);
        if (it != db.doublyLists.end()) {
            LREMOVE_HEAD(it->second);
            saveDatabaseToFile(filename, db);
            cout << "-> удалён первый элемент из списка '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "LREMOVE_TAIL") {
        auto it = db.doublyLists.find(structureName);
        if (it != db.doublyLists.end()) {
            LREMOVE_TAIL(it->second);
            saveDatabaseToFile(filename, db);
            cout << "-> удалён последний элемент из списка '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "LREMOVE_BEFORE") {
        string target; iss >> target;
        auto it = db.doublyLists.find(structureName);
        if (it != db.doublyLists.end()) {
            LREMOVE_BEFORE(it->second, target);
            saveDatabaseToFile(filename, db);
            cout << "-> удалён элемент перед " << target << " из списка '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "LREMOVE_AFTER") {
        string target; iss >> target;
        auto it = db.doublyLists.find(structureName);
        if (it != db.doublyLists.end()) {
            LREMOVE_AFTER(it->second, target);
            saveDatabaseToFile(filename, db);
            cout << "-> удалён элемент после " << target << " из списка '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "LREMOVE_BY_VALUE") {
        string value; iss >> value;
        auto it = db.doublyLists.find(structureName);
        if (it != db.doublyLists.end()) {
            bool ok = LREMOVE_BY_VALUE(it->second, value);
            saveDatabaseToFile(filename, db);
            if (ok)
                cout << "-> удалён " << value << " из списка '" << structureName << "'" << endl;
            else
                cout << "-> элемент " << value << " не найден в списке '" << structureName << "'" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "LFIND") {
        string value; iss >> value;
        auto it = db.doublyLists.find(structureName);
        if (it != db.doublyLists.end()) {
            LFIND(it->second, value);
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "LPRINT") {
        auto it = db.doublyLists.find(structureName);
        if (it != db.doublyLists.end()) {
            cout << "-> ";
            LPRINT(it->second);
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "LPRINT_FORWARD") {
        auto it = db.doublyLists.find(structureName);
        if (it != db.doublyLists.end()) {
            cout << "-> ";
            LPRINT_FORWARD(it->second);
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "LPRINT_BACKWARD") {
        auto it = db.doublyLists.find(structureName);
        if (it != db.doublyLists.end()) {
            cout << "-> ";
            LPRINT_BACKWARD(it->second);
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "LCLEAR") {
        auto it = db.doublyLists.find(structureName);
        if (it != db.doublyLists.end()) {
            LCLEAR(it->second);
            db.doublyLists.erase(it);
            saveDatabaseToFile(filename, db);
            cout << "-> список '" << structureName << "' очищен" << endl;
        } else {
            cout << "Список '" << structureName << "' не найден!" << endl;
        }
    }

    // ---- STACK ----
    else if (command == "SPUSH") {
        string value; iss >> value;
        SPUSH(db.stacks[structureName], value);
        saveDatabaseToFile(filename, db);
        cout << "-> добавлен " << value << " в стек '" << structureName << "'" << endl;
    }
    else if (command == "SPOP") {
        auto it = db.stacks.find(structureName);
        if (it != db.stacks.end()) {
            if (it->second.top) {
                SPOP(it->second);
                saveDatabaseToFile(filename, db);
                cout << "-> удалён верхний элемент из стека '" << structureName << "'" << endl;
            } else {
                cout << "Ошибка: стек '" << structureName << "' пуст" << endl;
            }
        } else {
            cout << "Стек '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "SPRINT") {
        auto it = db.stacks.find(structureName);
        if (it != db.stacks.end()) {
            cout << "-> ";
            SPRINT(it->second);
        } else {
            cout << "Стек '" << structureName << "' не найден!" << endl;
        }
    }
    else if (command == "SCLEAR") {
        auto it = db.stacks.find(structureName);
        if (it != db.stacks.end()) {
            SCLEAR(it->second);
            db.stacks.erase(it);
            saveDatabaseToFile(filename, db);
            cout << "-> стек '" << structureName << "' очищен" << endl;
        } else {
            cout << "Стек '" << structureName << "' не найден!" << endl;
        }
    }

    // ---- QUEUE ----
    else if (command == "QPUSH") {
        string value; iss >> value;
        QPUSH(db.queues[structureName], value);
        saveDatabaseToFile(filename, db);
        cout << "-> добавлен " << value << " в очередь '" << structureName << "'" << endl;
    }
    else if (command == "QPOP") {
        auto it = db.queues.find(structureName);
        if (it != db.queues.end()) {
            if (it->second.front) {
                QPOP(it->second);
                saveDatabaseToFile(filename, db);
                cout << "-> удалён первый элемент из очереди '" << structureName << "'" << endl;
            } else {
                cout << "Ошибка: очередь '" << structureName << "' пуста" << endl;
            }
        } else {
            cout << "Очередь '" << structureName << "' не найдена!" << endl;
        }
    }
    else if (command == "QPRINT") {
        auto it = db.queues.find(structureName);
        if (it != db.queues.end()) {
            cout << "-> ";
            QPRINT(it->second);
        } else {
            cout << "Очередь '" << structureName << "' не найдена!" << endl;
        }
    }
    else if (command == "QCLEAR") {
        auto it = db.queues.find(structureName);
        if (it != db.queues.end()) {
            QCLEAR(it->second);
            db.queues.erase(it);
            saveDatabaseToFile(filename, db);
            cout << "-> очередь '" << structureName << "' очищена" << endl;
        } else {
            cout << "Очередь '" << structureName << "' не найдена!" << endl;
        }
    }

    else {
        cerr << "Неизвестная команда: " << command << endl;
    }
}

// Функция для разбора аргументов командной строки
void parseCommandLine(int argc, char* argv[], string& filename, string& query) {
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) {
            filename = argv[++i];
        } else if (arg == "--query" && i + 1 < argc) {
            query = argv[++i];
        }
    }
}

int main(int argc, char* argv[]) {
    Commands cmd;
    string filename;
    string query;

    // Если переданы аргументы командной строки
    if (argc > 1) {
        parseCommandLine(argc, argv, filename, query);
        
        if (filename.empty() || query.empty()) {
            cerr << "Использование: " << argv[0] << " --file <filename> --query <command>" << endl;
            cerr << "Пример: " << argv[0] << " --file file.data --query 'SPUSH mystack item'" << endl;
            return 1;
        }
        
        // Выполняем команду
        cmd.execute(query, filename);
    } else {
        // Интерактивный режим
        cout << "Введите имя файла для работы: ";
        cin >> filename;
        cout << "Введите команды:" << endl;
        cin.ignore();

        while (true) {
            cout << "> ";
            getline(cin, query);
            
            if (query == "exit" || query == "quit") {
                break;
            }
            
            if (!query.empty()) {
                cmd.execute(query, filename);
            }
        }
    }

    return 0;
}
