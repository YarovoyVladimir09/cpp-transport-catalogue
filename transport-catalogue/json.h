#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

    struct RenderContext {
        RenderContext(std::ostream& out);
        RenderContext(std::ostream& out, int indent_step, int indent);
        RenderContext Indented() const;
        void RenderIndent() const;
        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    class Node;
    // —охраните объ€влени€ Dict и Array без изменени€
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // Ёта ошибка должна выбрасыватьс€ при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        using Value = std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict>;
        /* –еализуйте Node, использу€ std::variant */
        Node(nullptr_t value);
        Node();
        Node(int value);
        Node(std::string value);
        Node(double value);
        Node(bool value);
        Node(Array array);
        Node(Dict map);

        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsDict() const;
        bool IsMap() const;

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;
        const std::string& AsString() const;

        bool operator==(const Node& other) const;
        bool operator!=(const Node& other) const;
        bool operator==(Node& other) const;
        bool operator!=(Node& other) const;

        const Value GetValue()const {
            return node_inf;
        }

    private:
        Value node_inf;
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;


        bool operator==(const Document& other) const;
        bool operator!=(const Document& other) const;

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

    void PrintNode(const Node& node, const RenderContext& context);
}  // namespace json