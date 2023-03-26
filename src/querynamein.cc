#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <list>
#include <any>
#include <map>

class QueryNamed {
	// Static Values
	public:
		static const int UNKNOWN = 1;
		static const int NAMED = 2;
		static const int QUESTION = 3;
		static const int DOLLAR = 4;
		static const int AT = 5;
	// Methodes
	private:
	protected:
		std::string _originQuery;
		std::map<std::string, std::any> _originArgs;
		std::string _query;
		std::list<std::string> _names;
		std::list<std::any> _args;
		void Named(int bindType = QueryNamed::QUESTION);
	public:
		QueryNamed(const std::string &query, const std::map<std::string, std::any> &args = {}, int bindType = QueryNamed::QUESTION);
		std::string GetQuery(void);
		std::list<std::any> GetArgs(void);
		virtual std::list<std::any> Args(const std::map<std::string, std::any> &args);
};

QueryNamed::QueryNamed(const std::string &query, const std::map<std::string, std::any> &args, int bindType)
{
	this->_originQuery = query;
	this->_originArgs = args;
	this->Named(bindType);
	this->Args(args);
}

#define IsPermitChar(c) (std::isalpha(c) || std::isdigit(c) || c == '_')

void QueryNamed::Named(int bindType)
{
	bool inName = false;
	int last = this->_originQuery.size() - 1;
	int currentVar = 1;
	std::string name;

	for (int i = 0; i <= last ; i++) {
		char b = this->_originQuery[i];
		if (b == ':') {
			if (inName && i > 0 && this->_originQuery[i-1] == ':') {
				this->_query.append(1, ':');
				inName = false;
				continue;
			} else if (inName) {
				// Error
				std::cout << "TODO ERROR" << std::endl;
				goto error_clean;
			}
			inName = true;
			name.erase(name.begin(), name.end());
		} else if (inName && i > 0 && b == '=' && name.size() == 0) {
			this->_query.append(":=");
			inName = false;
			continue;
		} else if (inName && IsPermitChar(b) && i != last) {
			name.append(1, b);
		} else if (inName) {
			inName = false;
			if (i == last && IsPermitChar(b)) {
				name.append(1, b);
			}
			this->_names.push_back(name);
			switch (bindType) {
			case NAMED:
				this->_query.append(1, ':');
				this->_query.append(name);
				break;
			case UNKNOWN:
			case QUESTION:
				this->_query.append(1, '?');
				break;
			case DOLLAR:
				this->_query.append(1, '$');
				this->_query.append(std::to_string(currentVar));
				currentVar++;
				break;
			case AT:
				this->_query.append("@p");
				this->_query.append(std::to_string(currentVar));
				currentVar++;
				break;
			}
			if (i != last) {
				this->_query.append(1, b);
			} else if (!IsPermitChar(b)) {
				this->_query.append(1, b);
			}
		} else {
			this->_query.append(1, b);
		}
	}

	return;	
error_clean:
	this->_query = "";
	this->_names.erase(this->_names.begin(), this->_names.end());
}

std::string QueryNamed::GetQuery()
{
	return this->_query;
}

std::list<std::any> QueryNamed::GetArgs()
{
	return this->_args;
}

std::list<std::any> QueryNamed::Args(const std::map<std::string, std::any> &args)
{
	this->_originArgs = args;
	this->_args.erase(this->_args.begin(), this->_args.end());
	for (std::string name : this->_names) {
		auto it = args.find(name);
		if (it == args.end()) continue;
		this->_args.push_back(it->second);
	}
	if (this->_args.size() != this->_names.size()) {
		std::cout << "Error size" << std::endl;
	}
	return this->_args;
}

class QueryIn final : QueryNamed {
	private:
	protected:
		std::string _originInQuery;
		std::list<std::any> _originInArgs;
		std::string _Inquery;
		std::list<std::any> _args;
		void In(void);
	public:
		QueryIn(const std::string &query, const std::map<std::string, std::any> &args = {});
		QueryIn(const std::string &query, const std::list<std::any> &args);
		std::string GetQuery(void);
		std::list<std::any> GetArgs(void);
		virtual std::list<std::any> Args(const std::map<std::string, std::any> &args);
		std::list<std::any> Args(const std::list<std::string, std::any> &args);
};

QueryIn::QueryIn(const std::string &query, const std::map<std::string, std::any> &args):QueryNamed(query,args)
{
	//this->_originInQuery = query;
	//this->_originInArgs = args;
	//this->In();
}

QueryIn::QueryIn(const std::string &query, const std::list<std::any> &args):QueryNamed(query)
{
	this->_originInQuery = query;
	this->_originInArgs = args;
	this->In();
}

void QueryIn::In()
{
	// update query and update _args
	return;
}

std::string QueryIn::GetQuery()
{
	//
	return "toto";
}

std::list<std::any> QueryIn::GetArgs()
{
	//
	std::list<std::any> ret;
	return ret;
}

std::list<std::any> QueryIn::Args(const std::map<std::string, std::any> &args)
{
	//
	std::list<std::any> ret;
	return ret;
}

std::list<std::any> QueryIn::Args(const std::list<std::string, std::any> &args)
{
	//
	std::list<std::any> ret;
	return ret;
}


// TMP code for test

template<class T, class F>
inline std::pair<const std::type_index, std::function<void(std::any const&)>>
    to_any_visitor(F const &f)
{
    return {
        std::type_index(typeid(T)),
        [g = f](std::any const &a)
        {
            if constexpr (std::is_void_v<T>)
                g();
            else
                g(std::any_cast<T const&>(a));
        }
    };
}

static std::unordered_map<
    std::type_index, std::function<void(std::any const&)>>
    any_visitor {
        to_any_visitor<void>([]{ std::cout << "{}"; }),
        to_any_visitor<int>([](int x){ std::cout << x; }),
        to_any_visitor<unsigned>([](unsigned x){ std::cout << x; }),
        to_any_visitor<float>([](float x){ std::cout << x; }),
        to_any_visitor<double>([](double x){ std::cout << x; }),
        to_any_visitor<char const*>([](char const *s)
            { std::cout << std::quoted(s); }),
        // ... add more handlers for your types ...
    };
 
inline void process(const std::any& a)
{
    if (const auto it = any_visitor.find(std::type_index(a.type()));
        it != any_visitor.cend()) {
        it->second(a);
    } else {
        std::cout << "Unregistered type "<< std::quoted(a.type().name());
    }
}

int main() {
	std::map<std::string, std::any> mymap = { {"id", 11}, {"name", "Sophie"}, {"date", "1992-08-23 22:00:00"} };

	//auto myInQuery = QueryIn("SELECT * FROM peopleadv WHERE name IN (:names) AND date_now > :date", mymap);
	// c = GetConnection
	// ret = c.Select(myquery);
	// ret = c.Select(myquery.GetQuery(), myquery.GetArgs());

	auto t_start = std::chrono::high_resolution_clock::now();
	auto mySimpleInsertQuery = QueryNamed("INSERT INTO peopleadv(id, name, date_now) VALUE (:id, :name, :date)", mymap, QueryNamed::AT);
	std::cout << mySimpleInsertQuery.GetQuery() << std::endl;
	auto args = mySimpleInsertQuery.GetArgs();
	auto t_end = std::chrono::high_resolution_clock::now();
	for (auto arg : args) {
		process(arg);
		std::cout << ",";
	}
	std::cout << std::endl;

	double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();
	std::cout << elapsed_time_ms << std::endl;

	// c = GetConnection
	// p = c.PrepareNamed(mySimpleInsertQuery.GetQuery())
	// foreach args := listOfArgs {
	//      ret = p.Exec(mysimpleInsertQuery.Args(args));
	// }

	//auto mySimpleSelectQuery = QueryNamed("SELECT * FROM peopleadv WHERE name LIKE :name AND date_now > :date", mymap);
	// c = GetConnection
	// c.PrepareNamed(mySimpleSelectQuery)
	// ret = c.Select(args);
	// // Print all


	return 0;
}
