; ModuleID = 'test.ll'
source_filename = "../test.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx15.0.0"

; Function Attrs: nofree norecurse nosync nounwind ssp memory(argmem: write) uwtable(sync)
define void @work(ptr noundef writeonly %0, i32 noundef %1, i32 noundef %2) local_unnamed_addr #0 {
  %4 = icmp sgt i32 %1, 0
  br i1 %4, label %5, label %.loopexit2

5:                                                ; preds = %3
  %6 = icmp sgt i32 %2, 0
  %7 = zext i32 %2 to i64
  %8 = zext nneg i32 %1 to i64
  br label %9

9:                                                ; preds = %23, %5
  %10 = phi i64 [ 0, %5 ], [ %14, %.loopexit ]
  br i1 %6, label %11, label %.loopexit

11:                                               ; preds = %9
  %12 = mul nuw nsw i64 %10, %7
  %13 = getelementptr inbounds nuw i32, ptr %0, i64 %12
  br label %16

.loopexit2:                                       ; preds = %.loopexit, %3
  ret void

.loopexit:                                        ; preds = %23, %16, %9
  %14 = add nuw nsw i64 %10, 1
  %15 = icmp eq i64 %14, %8
  br i1 %15, label %.loopexit2, label %23, !llvm.loop !6

16:                                               ; preds = %16, %11
  %17 = phi i64 [ 0, %11 ], [ %21, %16 ]
  %18 = add nuw nsw i64 %17, %10
  %19 = getelementptr inbounds nuw i32, ptr %13, i64 %17
  %20 = trunc nuw i64 %18 to i32
  store i32 %20, ptr %19, align 4, !tbaa !9
  %21 = add nuw nsw i64 %17, 1
  %22 = icmp eq i64 %21, %7
  br i1 %22, label %.loopexit, label %16, !llvm.loop !13

23:                                               ; preds = %.loopexit
  %24 = phi i64 [ 0, %11 ], [ %28, %23 ]
  %25 = add nuw nsw i64 %24, %10
  %26 = getelementptr inbounds nuw i32, ptr %13, i64 %24
  %27 = trunc nuw i64 %25 to i32
  store i32 %27, ptr %26, align 4, !tbaa !9
  %28 = add nuw nsw i64 %24, 1
  %29 = icmp eq i64 %28, %7
  br i1 %29, label %.loopexit, label %9, !llvm.loop !13
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
