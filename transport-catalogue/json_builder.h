#pragma once

#include "json.h"

#include <algorithm>

namespace json {
	
	class Builder {
		friend Node;
	public:
		class DictItemContext;
		class KeyContext;
		class BaseContext;
		class ArrayItemContext;
		class ValueDictContext;

		Builder() = default;
		KeyContext Key(std::string key);
		BaseContext Value(Node value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		BaseContext EndDict();
		BaseContext EndArray();
		Node Build();

	private:
		Node* root_ = nullptr;
		std::string key_buffer ;
		std::vector<Node*> nodes_stack_;
		bool ready = false;
		bool key_done = false;

	public:
		class BaseContext
		{
		public:
			BaseContext(Builder& build) :builder(build) {

			}
			KeyContext Key(std::string key) {
				builder.Key(key);
				return builder;
			}
			BaseContext Value(Node value) {
				return builder.Value(value);
				
			}
			DictItemContext StartDict() {
				builder.StartDict();
				return builder;
			}
			ArrayItemContext StartArray() {
				builder.StartArray();
				return builder;
			}
			BaseContext EndDict() {
				builder.EndDict();
				return builder;
			}
			BaseContext EndArray() {
				builder.EndArray();
				return builder;
			}
			Node Build() {
				return builder.Build();
			}


			Builder& builder;
		};

		class DictItemContext :public BaseContext
		{
		public:
			DictItemContext(Builder& build) :BaseContext(build) {

			}
			DictItemContext& Value(Node value) = delete;
			DictItemContext& StartDict() = delete;
			DictItemContext& StartArray() = delete;
			DictItemContext& EndArray() = delete;
			Node Build() = delete;
		};

		class KeyContext :public BaseContext
		{
		public:
			KeyContext(Builder& build) :BaseContext(build) {

			}
			ValueDictContext Value(Node value) {
				builder.Value(value);
				return builder;
			}
			Builder& Key(std::string key) = delete;
			KeyContext& EndDict() = delete;
			KeyContext& EndArray() = delete;
			Node Build() = delete;
		};

		class ArrayItemContext :public BaseContext
		{
		public:
			ArrayItemContext(Builder& build) :BaseContext(build) {

			}
			ArrayItemContext Value(Node value) {
				builder.Value(value);
				return builder;
			}
			Builder& Key(std::string key) = delete;
			KeyContext& EndDict() = delete;
			Node Build() = delete;
		};

		class ValueDictContext :public BaseContext
		{
		public:
			ValueDictContext(Builder& build) :BaseContext(build) {

			}
			Node Build() = delete;
			Builder& Value(Node value) = delete;
			DictItemContext StartDict() = delete;
			ArrayItemContext StartArray() = delete;
			Builder& EndArray() = delete;
		};
	};


}