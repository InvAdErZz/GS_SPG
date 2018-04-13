#pragma once


class Non_copyable
{
private:
	auto operator=(Non_copyable const&)->Non_copyable& = delete;
	Non_copyable(Non_copyable const&) = delete;
public:
	auto operator=(Non_copyable&&)->Non_copyable& = default;
	Non_copyable() = default;
	Non_copyable(Non_copyable&&) = default;
};

template<class FunctionType>
class Scope_guard
	: public Non_copyable
{
private:
	FunctionType cleanup_;

public:
	friend
		void dismiss(Scope_guard& g) { g.cleanup_ = [] {}; }

	~Scope_guard() { cleanup_(); }

	template< class Func >
	Scope_guard(Func const& cleanup)
		: cleanup_(cleanup)
	{}

	Scope_guard(Scope_guard&& other)
		: cleanup_(move(other.cleanup_))
	{
		dismiss(other);
	}
};

template<class FunctionType>
Scope_guard MakeScopeGuard(FunctionType func)
{
	return Scope_guard<FunctionType>(func)
}