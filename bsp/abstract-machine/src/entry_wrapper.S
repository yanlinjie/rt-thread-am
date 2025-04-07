# .section .text
# .global entry_wrapper
# .extern debug_print_hex    # 声明外部 C 函数
# entry_wrapper:
#   mv a7, a0       # 把 parameter 移动到 a7，防止被 tentry 覆盖（可选）
#   mv a0, a2         # 把 texit 放到 a0 中，作为参数传给 C 函数
#   # call debug_print_hex  # 调试用，打印 texit 的值
#   mv a0, a7       # 恢复 a0 = parameter
#   jalr a1         # 跳转并链接到 tentry(parameter)
  

#   # 返回后继续执行 texit()
#   jalr a2         # 调用 texit()，该函数永不返回
