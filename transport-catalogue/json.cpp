#include "json.h"

using namespace std;

namespace json {

    namespace {

        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            Array result;
            bool ch_ = false;
            for (char c; input >> c;) {
                if (c == ']') {
                    ch_ = true;
                    break;
                }
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (!ch_) {
                throw ParsingError("Failed to ]"s);
            }

            return Node(move(result));
        }

        Node LoadNumber(istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadBool(istream& input, char inf) {
            string s;
            char c;
            string check = "falsetrue";
            while (input >> c) {
                if (check.find(c)==check.npos) {
                    input.putback(c);
                    break;
                }
                s += c;
            }
            if (s != "alse" && s != "rue") {
                throw ParsingError("Incorrect word"s);

            }
            return inf == 't' ? Node(true) : Node(false);

        }

        Node LoadNull(istream& input) {
            string s;
            char c;
            while (input >> c) {
                if (c == ' ' || c == ',') {
                    break;
                }
                s += c;
            }
            if (s != "ull") {
                throw ParsingError("Incorrect word"s);
            }
            return Node();
        }

        Node LoadString(istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return s;
        }

        Node LoadDict(istream& input) {
            Dict result;
            bool ch_ = false;
            for (char c; input >> c;) {
                if (c == '}') {
                    ch_ = true;
                    break;
                }
                if (c == ',') {
                    input >> c;
                }

                string key = LoadString(input).AsString();
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }
            if (!ch_) {
                throw ParsingError("Failed to }"s);
            }

            return Node(move(result));
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else if (c == 't') {
                return LoadBool(input, 't');
            }
            else if (c == 'f') {
                return LoadBool(input, 'f');
            }
            else if (c == 'n') {
                return LoadNull(input);
            }
            else if (c == EOF)
            {
                throw ParsingError("Failed input"s);
            }
            else {
                input.putback(c);
                return LoadNumber(input);
            }
        }

    }  // namespace

    // ---------------------CONSTR------------------
    Node::Node() {
        node_inf = nullptr;
    }

    Node::Node(nullptr_t value)
        :node_inf(value) {
    }

    Node::Node(Array array)
        : node_inf(move(array)) {
    }

    Node::Node(int value)
        : node_inf(value) {
    }

    Node::Node(string value)
        : node_inf(move(value)) {
    }

    Node::Node(double value)
        : node_inf(value) {
    }

    Node::Node(bool value)
        : node_inf(move(value)) {
    }

    Node::Node(Dict map)
        : node_inf(move(map)) {
    }

    //-------------------------IS-------------------

    bool Node::IsInt() const {
        if (holds_alternative<int>(node_inf)) {
            return true;
        }
        return false;
    }
    bool Node::IsDouble() const {
        if (holds_alternative<int>(node_inf) || holds_alternative<double>(node_inf)) {
            return true;
        }
        return false;
    }
    bool Node::IsPureDouble() const {
        if (holds_alternative<double>(node_inf)) {
            return true;
        }
        return false;
    }
    bool Node::IsBool() const {
        if (holds_alternative<bool>(node_inf)) {
            return true;
        }
        return false;
    }
    bool Node::IsString() const {
        if (holds_alternative<string>(node_inf)) {
            return true;
        }
        return false;
    }
    bool Node::IsNull() const {
        if (holds_alternative<nullptr_t>(node_inf)) {
            return true;
        }
        return false;
    }
    bool Node::IsArray() const {
        if (holds_alternative<Array>(node_inf)) {
            return true;
        }
        return false;
    }
    bool Node::IsDict() const {
        if (holds_alternative<Dict>(node_inf)) {
            return true;
        }
        return false;
    }
    bool Node::IsMap() const {
        if (holds_alternative<Dict>(node_inf)) {
            return true;
        }
        return false;
    }

    //-------------------------------AS-----------------

    const Array& Node::AsArray() const {
        if (IsArray()) return get<Array>(node_inf);
        else {
            throw logic_error("wrong");
        }
    }

    const Dict& Node::AsMap() const {
        if (IsDict()) return get<Dict>(node_inf);
        else {
            throw logic_error("wrong");
        }
    }

    int Node::AsInt() const {
        if (IsInt())return get<int>(node_inf);
        else {
            throw logic_error("wrong");
        }
    }

    bool Node::AsBool() const {
        if (IsBool()) return get<bool>(node_inf);
        else {
            throw logic_error("wrong");
        }
    }

    double Node::AsDouble() const {

        if (IsPureDouble()) {
            return get<double>(node_inf);
        }
        else if (IsInt()) {
            return static_cast<double>(get<int>(node_inf));
        }
        else {
            throw logic_error("wrong");
        }
    }

    const string& Node::AsString() const {
        if (IsString()) return get<string>(node_inf);
        else {
            throw logic_error("wrong");
        }
    }

    //------------------OPERATOR------------------

    bool Node::operator==(const Node& other) const {
        if (node_inf.index() == other.node_inf.index()) {
            return node_inf == other.node_inf;
        }
        return false;
    }
    bool Node::operator!=(const Node& other) const {
        return !(*this == other);
    }

    bool Node::operator==(Node& other) const {
        if (node_inf.index() == other.node_inf.index()) {
            return node_inf == other.node_inf;
        }
        return false;
    }
    bool Node::operator!=(Node& other) const {
        return !(*this == other);
    }

    //-----------DOC---------------

    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    bool Document::operator==(const Document& other) const {
        return root_ == other.root_;
    }
    bool Document::operator!=(const Document& other) const {
        return root_ != other.root_;
    }

    //-------------------PRINT--------------

    template <typename Value>
    void PrintValue(const Value& value, const RenderContext& context) {
        context.RenderIndent();
        auto& out = context.out;
        out << value;
    }

    void PrintValue(const bool& value, const RenderContext& context) {
        context.RenderIndent();
        auto& out = context.out;
        out << boolalpha << value;
    }

    void PrintValue(std::nullptr_t,const RenderContext& context) {
        context.RenderIndent();
        auto& out = context.out;
        out << "null"sv;
    }

    void PrintValue(Array arr, const RenderContext& context) {
        auto& out = context.out;
        size_t i = 0;
        context.RenderIndent();
        out << "["<<endl;
        auto context_ = context.Indented();
        for (auto& node_ : arr) {
            PrintNode(node_, context_);
            if (i != arr.size() - 1) {
                out << ",";
            }
            ++i;
            out << endl;
        }
        context.RenderIndent();
        out << "]";
    }
    void PrintValue(Dict dict, const RenderContext& context) {
        auto& out = context.out;
        size_t i = 0;
        
        context.RenderIndent();
        out << "{" << endl;
        auto context_ = context.Indented();
        for (auto& node_ : dict) {
            PrintNode(node_.first, context_);
            out << ": ";
            if (node_.second.IsDict() || node_.second.IsArray()) {
                out << endl;
                PrintNode(node_.second, context_);
            }
            else {
                PrintNode(node_.second, RenderContext(out));
            }
            if (i != dict.size() - 1) {
                out << ",";
            }
            out << endl;
            ++i;
        }
        context.RenderIndent();
        out << "}";
    }

    void PrintValue(const string& value, const RenderContext& context) {
        context.RenderIndent();
        auto& out = context.out;
        out << "\"";
        for (auto& word : value) {
            if (word == '\\') {
                out << "\\\\";
            }
            else if (word == '\"') {
                out << "\\\"";
            }
            else if (word == '\n') {
                out << "\\n";
            }
            else if (word == '\r') {
                out << "\\r";
            }
            else {
                out << word;
            }
        }
        out << "\"";
    }
    void Print(const Document& doc, std::ostream& output) {
        PrintNode(doc.GetRoot(), RenderContext(output));
    }

    void PrintNode(const Node& node,const RenderContext& context) {
        std::visit(
            [&context](const auto& value) { PrintValue(value, context); },
            node.GetValue());
    }
}  