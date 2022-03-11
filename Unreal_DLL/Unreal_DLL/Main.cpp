#include <iostream>
#include "Common/BitVector.h"
#include "Common/LinkedList.h"
#include"CJsonObject/CJsonObject.hpp"
#include "Utility/StringUtility.h"

#include "JobSystem/JobInclude.h"
#include "Common/HashMap.h"
#include "Utility/FileUtility.h"

class MiDLL
{
private:
	HashMap<
		vengine::string, 
		HashMap<vengine::string, void*>
	> funcPtrs;
	template <typename T>
	struct GetFuncPointer;

	template <typename Ret, typename ... Args>
	struct GetFuncPointer<Ret(Args...)>
	{
		using FuncPtr = typename Ret(*)(Args...);
	};
	template <typename T>
	using GetFuncPointer_t = typename GetFuncPointer<T>::FuncPtr;
public:
	MiDLL()
	{}
	~MiDLL()
	{}
	static MiDLL* GetInstance()
	{
		return new MiDLL;
	}
	template<typename T>
	GetFuncPointer_t<T> GetFunctionPointer(
		const vengine::string& PluginPath, const vengine::string& DLLPath, const vengine::string& FunctionName
	)
	{
		vengine::string path = PluginPath + DLLPath; // TODO
		auto ite = funcPtrs.Find(path);
		if (!ite)
		{
			//Cannot find dll
			return nullptr;
		}
		auto hashIte = ite.Value().Find(FunctionName);
		if (!hashIte)
		{
			//Cannot find function
			return nullptr;
		}

		return static_cast<GetFuncPointer_t<T>>(hashIte.Value());
	}

	void AddNewDLL(
		vengine::string const& PluginPath,
		vengine::string const& DLLPath,
		const vengine::string& FunctionName,
		FARPROC ptr
	)
	{
		vengine::string path = PluginPath + DLLPath; // TODO
		auto ite = funcPtrs.Insert(path);
		ite.Value().Insert(FunctionName, ptr);
	}

	/*template <typename T, typename ... Args>
	T Use(const vengine::string& PluginPath, const vengine::string& DLLPath, const vengine::string& FunctionName, Args&&... args)
	{
		using TA = T(*)(Args);
		TA tt;
		T r = (*tt)(std::forward<Args>(args)...);
		return r;
	}
	*/

};

void DLL(int a,int b,bool c)
{}
template<typename T>
struct IsArray
{
	static constexpr bool value = false;
};

template<typename T, size_t s>
struct IsArray<T[s]>
{
	static constexpr bool value = true;
};

int main()
{
	//int x;
	//int xy[5];
	//IsArray<decltype(x)>::value;
	////IsArray<decltype(xy)>::v;
	///*using namespace std;
	//JobSystem system(std::thread::hardware_concurrency());
	//JobBucket* bucket = system.GetJobBucket();


	//JobHandle handle0 = bucket->GetTask({}, []()->void
	//{
	//	double x = 1;
	//	for (int i = 0; i < 1000000; ++i)
	//	{
	//		x = pow(2, x) + 0.01f;
	//	}
	//	cout << "Job 0 Finished" << endl;
	//});

	//JobHandle handle1 = bucket->GetTask({}, []()->void
	//{
	//	double x = 1;
	//	for (int i = 0; i < 1000000; ++i)
	//	{
	//		x = pow(2, x) + 0.01f;
	//	}
	//	cout << "Job 1 Finished" << endl;
	//});
	//
	//JobHandle handle2 = bucket->GetTask({ handle0, handle1 }, []()->void 
	//{
	//	cout << "They are all finished!" << endl;
	//});
	//JobHandle parallelHandle = bucket->GetParallelTask({ handle2 }, 100, 100, [](uint kernel)->void
	//{
	//	double x = 1;
	//	for (int i = 0; i < 1000000; ++i)
	//	{
	//		x = pow(2, x) + 0.01f;
	//	}
	//	cout << kernel << endl;
	//});

	//system.ExecuteBucket(bucket, 1);
	//system.Wait();*/
	//void(*funcPtr)(int, double);
	//funcPtr = MiDLL::GetInstance()->GetFunctionPointer<void(int, double)>("pluginPath","dllPath","FunctionName");
	////funcPtr(,)
}
