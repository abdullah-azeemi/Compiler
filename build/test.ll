; ModuleID = '../test.c'
source_filename = "../test.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx15.0.0"

; Function Attrs: nofree norecurse nosync nounwind ssp memory(argmem: write) uwtable(sync)
define void @work(ptr noundef writeonly %0, i32 noundef %1, i32 noundef %2) local_unnamed_addr #0 {
  %4 = icmp sgt i32 %1, 0
  br i1 %4, label %5, label %15

5:                                                ; preds = %3
  %6 = icmp sgt i32 %2, 0
  %7 = zext i32 %2 to i64
  %8 = zext nneg i32 %1 to i64
  %9 = zext nneg i32 %2 to i64
  br label %10

10:                                               ; preds = %5, %16
  %11 = phi i64 [ 0, %5 ], [ %17, %16 ]
  br i1 %6, label %12, label %16

12:                                               ; preds = %10
  %13 = mul nuw nsw i64 %11, %7
  %14 = getelementptr inbounds i32, ptr %0, i64 %13
  br label %19

15:                                               ; preds = %16, %3
  ret void

16:                                               ; preds = %19, %10
  %17 = add nuw nsw i64 %11, 1
  %18 = icmp eq i64 %17, %8
  br i1 %18, label %15, label %10, !llvm.loop !6

19:                                               ; preds = %12, %19
  %20 = phi i64 [ 0, %12 ], [ %24, %19 ]
  %21 = add nuw nsw i64 %20, %11
  %22 = getelementptr inbounds i32, ptr %14, i64 %20
  %23 = trunc nuw i64 %21 to i32
  store i32 %23, ptr %22, align 4, !tbaa !9
  %24 = add nuw nsw i64 %20, 1
  %25 = icmp eq i64 %24, %9
  br i1 %25, label %16, label %19, !llvm.loop !13
}

attributes #0 = { nofree norecurse nosync nounwind ssp memory(argmem: write) uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 15, i32 5]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 8, !"PIC Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 1}
!5 = !{!"Homebrew clang version 21.1.6"}
!6 = distinct !{!6, !7, !8}
!7 = !{!"llvm.loop.mustprogress"}
!8 = !{!"llvm.loop.unroll.disable"}
!9 = !{!10, !10, i64 0}
!10 = !{!"int", !11, i64 0}
!11 = !{!"omnipotent char", !12, i64 0}
!12 = !{!"Simple C/C++ TBAA"}
!13 = distinct !{!13, !7, !8}
