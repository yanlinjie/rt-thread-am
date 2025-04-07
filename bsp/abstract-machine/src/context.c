#include <am.h>
#include <klib.h>
#include <rtthread.h>
#define STACK_SIZE 16384

static Context* ev_handler(Event e, Context *c) {
  switch (e.event) {
    case EVENT_YIELD:  {
      rt_thread_t cp = rt_thread_self(); 
      rt_ubase_t to = cp->user_data;
      c = *(Context**)to;
      // c->mepc += 4;
      break;
    }
    default: printf("Unhandled event ID = %d\n", e.event); assert(0);
  }
  return c;
}

void __am_cte_init() {
  cte_init(ev_handler);
}



void rt_hw_context_switch_to(rt_ubase_t to) {
  rt_thread_t pcb = rt_thread_self();
  rt_ubase_t user_data_replicator = pcb->user_data;
  pcb->user_data = to;
  yield();
  pcb->user_data = user_data_replicator;
}

void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to) {
  rt_thread_t pcb = rt_thread_self();
  rt_ubase_t user_data_replicator = pcb->user_data;
  pcb->user_data = to;
  *(Context**)from = pcb->sp;
  yield();
  pcb->user_data = user_data_replicator;
}



void rt_hw_context_switch_interrupt(void *context, rt_ubase_t from, rt_ubase_t to, struct rt_thread *to_thread) {
  assert(0);
}
void wrap(void *arg) {
  // parse paramater
  rt_ubase_t *stack_bottom = (rt_ubase_t *)arg;
  rt_ubase_t tentry = *stack_bottom; 
  stack_bottom--;
  rt_ubase_t texit = *stack_bottom; 
  stack_bottom--;
  rt_ubase_t parameter = *stack_bottom; 
  // function call
  ((void(*)())tentry) (parameter);
  ((void(*)())texit) ();
}

rt_uint8_t *rt_hw_stack_init(void *tentry, void *parameter, rt_uint8_t *stack_addr, void *texit) {
  // align
  stack_addr = (rt_uint8_t*)((uintptr_t)stack_addr & ~(sizeof(uintptr_t) - 1));
  // create context
  Area stack;
  stack.start = stack_addr - STACK_SIZE;
  stack.end = stack_addr;
  Context *cp = kcontext(stack, wrap, (void *)(stack.end - sizeof(Context) - 1));
  // set parameter
  rt_ubase_t *stack_bottom = (rt_ubase_t *)(stack.end - sizeof(Context) - 1);
  *stack_bottom = (rt_ubase_t)tentry;
  stack_bottom--;
  *stack_bottom = (rt_ubase_t)texit;
  stack_bottom--;
  *stack_bottom = (rt_ubase_t)parameter;

  return (rt_uint8_t *)cp;
}
