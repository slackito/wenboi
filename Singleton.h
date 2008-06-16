// Meyers' singleton
// from http://www.devarticles.com/c/a/Cplusplus/C-plus-plus-In-Theory-The-Singleton-Pattern-Part-2/1/
#if !defined(SINGLETON_H)
#define SINGLETON_H

template <typename T>
class Singleton
{
	public:
	static T& Instance() 
	{
		static T _instance;
		return _instance;
	}

	protected:
		Singleton() {}          // ctor hidden
		virtual ~Singleton() {}          // dtor hidden
		Singleton(Singleton const&);    // copy ctor hidden
		Singleton& operator=(Singleton const&);  // assign op hidden
};

#endif
