; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.11.0"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @work(i32* %A, i32 %N, i32 %M) #0 {
entry:
  %M.addr = alloca i32, align 4
  %N.addr = alloca i32, align 4
  %A.addr = alloca i32*, align 8
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  store i32 %M, i32* %M.addr, align 4
  store i32 %N, i32* %N.addr, align 4
  store i32* %A, i32** %A.addr, align 8
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc5, %entry
  %0 = load i32, i32* %i, align 4
  %1 = load i32, i32* %N.addr, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end7

for.body:                                         ; preds = %for.cond
  store i32 0, i32* %j, align 4
  br label %for.cond1

for.cond1:                                        ; preds = %for.inc, %for.body
  %2 = load i32, i32* %j, align 4
  %3 = load i32, i32* %M.addr, align 4
  %cmp2 = icmp slt i32 %2, %3
  br i1 %cmp2, label %for.body3, label %for.end

for.body3:                                        ; preds = %for.cond1
  %4 = load i32, i32* %i, align 4
  %5 = load i32, i32* %j, align 4
  %add = add nsw i32 %4, %5
  %6 = load i32*, i32** %A.addr, align 8
  %7 = load i32, i32* %i, align 4
  %8 = load i32, i32* %M.addr, align 4
  %mul = mul nsw i32 %7, %8
  %9 = load i32, i32* %j, align 4
  %add4 = add nsw i32 %mul, %9
  %idxprom = sext i32 %add4 to i64
  %arrayidx = getelementptr inbounds i32, i32* %6, i64 %idxprom
  store i32 %add, i32* %arrayidx, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body3
  %10 = load i32, i32* %j, align 4
  %inc = add nsw i32 %10, 1
  store i32 %inc, i32* %j, align 4
  br label %for.cond1

for.end:                                          ; preds = %for.cond1
  br label %for.inc5

for.inc5:                                         ; preds = %for.end
  %11 = load i32, i32* %i, align 4
  %inc6 = add nsw i32 %11, 1
  store i32 %inc6, i32* %i, align 4
  br label %for.cond

for.end7:                                         ; preds = %for.cond
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 2}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"clang version 12.0.0"}
