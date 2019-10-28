  .globl default_handler
default_handler:
  jmp default_handler

  .globl kbc_handler
kbc_handler:
  push %rax
  push %rcx
  push %rdx
  push %rbx
  push %rbp
  push %rsi
  push %rdi
  call do_kbc_interrupt
  pop %rdi
  pop %rsi
  pop %rbp
  pop %rbx
  pop %rdx
  pop %rcx
  pop %rax
  iretq

  .globl hpet_handler
hpet_handler:
  push %rax
  push %rcx
  push %rdx
  push %rbx
  push %rbp
  push %rsi
  push %rdi
  mov %rsp, %rdi
  call do_hpet_interrupt
  pop %rdi
  pop %rsi
  pop %rbp
  pop %rbx
  pop %rdx
  pop %rcx
  pop %rax
  iretq

  .globl syscall_handler
syscall_handler:
  push %rcx
  push %rax
  push %rdx
  push %rbx
  push %rbp
  push %rsi
  push %rdi
  call do_syscall_handler
  pop %rdi
  pop %rsi
  pop %rbp
  pop %rbx
  pop %rdx
  pop %rcx
  pop %rcx
  iretq
