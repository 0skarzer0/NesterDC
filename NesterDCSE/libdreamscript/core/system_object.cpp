#include "system_object.h"
#include "../kjs/lookup.h"
#include "../kjs/collector.h"
#include "system_object.lut.h"

#include "dc_utils.h"

const ClassInfo SystemImp::info = { "System", 0, &systemTable, 0 };

SystemImp* g_systemImp = NULL;

/* Source for system_object.lut.h

enum { E_toString, E_availableMemory, E_garbageCollect, E_startTimer, E_stopTimer };

@begin systemTable 4
	toString			SystemImp::E_toString				DontDelete|ReadOnly|Function	0
	availableMemory		SystemImp::E_availableMemory		DontDelete|ReadOnly|Function	0
	garbageCollect		SystemImp::E_garbageCollect			DontDelete|ReadOnly|Function	0
	startTimer			SystemImp::E_startTimer				DontDelete|ReadOnly|Function	2
	stopTimer			SystemImp::E_stopTimer				DontDelete|ReadOnly|Function	0
@end
*/

SystemImp::SystemImp() : ObjectImp()
{

}

Value SystemImp::get(ExecState *exec, const Identifier &propertyName) const
{
	return lookupGet<SystemFunctionImp, SystemImp, ObjectImp>(exec, propertyName, &systemTable, this);
}

Value SystemImp::getValueProperty(ExecState *exec, int token) const
{
	return Undefined();
}

SystemFunctionImp::SystemFunctionImp(ExecState* exec, int i, int len) : ObjectImp()
{
	id = i;
	put(exec, lengthPropertyName, Number(len), DontDelete|ReadOnly|DontEnum);
}

Object g_TimerHandlerFunc;
Object g_TimerThisObj;
ExecState* g_TimerExec = NULL;

static void timer_handler(irq_t source, irq_context_t *context)
{
	if (!g_TimerExec)
		return;
	g_TimerHandlerFunc.call(g_TimerExec, g_TimerThisObj, List());
}

Value SystemFunctionImp::call(ExecState *exec, Object &thisObj, const List &args)
{
	if (thisObj.className() != "System")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	switch (id)
	{
		case SystemImp::E_toString:
		{
			return String("System Object");
			break;
		}
		case SystemImp::E_availableMemory:
		{
			struct mallinfo info = mallinfo();
			return Number(info.fordblks);
			break;
		}
		case SystemImp::E_garbageCollect:
		{
			Collector::collect();
			break;
		}
		case SystemImp::E_startTimer:
		{
			g_TimerExec = exec;
			g_TimerThisObj = thisObj;
			unsigned int cycles_per_second = args[0].toUInt32(exec);
			g_TimerHandlerFunc = args[1].toObject(exec);
			
			timer_prime(TMU2, cycles_per_second, true);
			irq_set_handler(EXC_TMU2_TUNI2, timer_handler);
			timer_start(TMU2);
			break;
		}
		case SystemImp::E_stopTimer:
		{
			timer_stop(TMU2);
			g_TimerExec = NULL;
			break;
		}
	}
	return Undefined();
}

void InitSystemObject(ExecState* exec, Object obj)
{
	g_systemImp = new SystemImp();
	obj.put(exec, Identifier("System"), Object(g_systemImp));
}
