; ModuleID = 'cminus'
source_filename = "/home/dawn/workspace/2025ustc-compiler-new/tests/2-ir-gen/autogen/testcases/lv3/complex3.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define float @get(float* %arg0, i32 %arg1, i32 %arg2, i32 %arg3) {
label_entry:
  %op4 = alloca float*
  store float* %arg0, float** %op4
  %op5 = alloca i32
  store i32 %arg1, i32* %op5
  %op6 = alloca i32
  store i32 %arg2, i32* %op6
  %op7 = alloca i32
  store i32 %arg3, i32* %op7
  %op8 = load i32, i32* %op5
  %op9 = load i32, i32* %op7
  %op10 = mul i32 %op8, %op9
  %op11 = load i32, i32* %op6
  %op12 = add i32 %op10, %op11
  %op13 = icmp slt i32 %op12, 0
  br i1 %op13, label %label18, label %label14
label14:                                                ; preds = %label_entry, %label18
  %op15 = load float*, float** %op4
  %op16 = getelementptr float, float* %op15, i32 %op12
  %op17 = load float, float* %op16
  ret float %op17
label18:                                                ; preds = %label_entry
  call void @neg_idx_except()
  br label %label14
}
define float @abs(float %arg0) {
label_entry:
  %op1 = alloca float
  store float %arg0, float* %op1
  %op2 = load float, float* %op1
  %op3 = sitofp i32 0 to float
  %op4 = fcmp ugt float %op2, %op3
  %op5 = zext i1 %op4 to i32
  %op6 = icmp ne i32 %op5, 0
  br i1 %op6, label %label7, label %label10
label7:                                                ; preds = %label_entry
  %op8 = load float, float* %op1
  ret float %op8
label9:
  ret float 0x0
label10:                                                ; preds = %label_entry
  %op11 = load float, float* %op1
  %op12 = sitofp i32 0 to float
  %op13 = fsub float %op12, %op11
  ret float %op13
}
define float @isZero(float %arg0) {
label_entry:
  %op1 = alloca float
  store float %arg0, float* %op1
  %op2 = load float, float* %op1
  %op3 = call float @abs(float %op2)
  %op4 = fcmp ult float %op3, 0x3eb0c6f7a0000000
  %op5 = zext i1 %op4 to i32
  %op6 = sitofp i32 %op5 to float
  ret float %op6
}
define i32 @gauss(float* %arg0, float* %arg1, i32 %arg2) {
label_entry:
  %op3 = alloca float*
  store float* %arg0, float** %op3
  %op4 = alloca float*
  store float* %arg1, float** %op4
  %op5 = alloca i32
  store i32 %arg2, i32* %op5
  %op6 = alloca i32
  %op7 = alloca i32
  %op8 = alloca i32
  %op9 = alloca i32
  %op10 = alloca i32
  %op11 = alloca i32
  %op12 = alloca float
  %op13 = load i32, i32* %op5
  %op14 = add i32 %op13, 1
  store i32 %op14, i32* %op9
  store i32 0, i32* %op6
  br label %label_judgeBB0
label_judgeBB0:                                                ; preds = %label_entry, %label22
  %op15 = load i32, i32* %op6
  %op16 = load i32, i32* %op5
  %op17 = icmp slt i32 %op15, %op16
  %op18 = zext i1 %op17 to i32
  %op19 = icmp sgt i32 %op18, 0
  br i1 %op19, label %label_stateBB0, label %label_outBB0
label_stateBB0:                                                ; preds = %label_judgeBB0
  %op20 = load i32, i32* %op6
  %op21 = icmp slt i32 %op20, 0
  br i1 %op21, label %label28, label %label22
label_outBB0:                                                ; preds = %label_judgeBB0
  store i32 0, i32* %op11
  store i32 0, i32* %op8
  br label %label_judgeBB1
label22:                                                ; preds = %label_stateBB0, %label28
  %op23 = load float*, float** %op3
  %op24 = getelementptr float, float* %op23, i32 %op20
  %op25 = sitofp i32 0 to float
  store float %op25, float* %op24
  %op26 = load i32, i32* %op6
  %op27 = add i32 %op26, 1
  store i32 %op27, i32* %op6
  br label %label_judgeBB0
label28:                                                ; preds = %label_stateBB0
  call void @neg_idx_except()
  br label %label22
label_judgeBB1:                                                ; preds = %label_outBB0, %label120
  %op29 = load i32, i32* %op8
  %op30 = load i32, i32* %op5
  %op31 = icmp slt i32 %op29, %op30
  %op32 = zext i1 %op31 to i32
  %op33 = icmp sgt i32 %op32, 0
  br i1 %op33, label %label_stateBB1, label %label_outBB1
label_stateBB1:                                                ; preds = %label_judgeBB1
  %op34 = load i32, i32* %op8
  store i32 %op34, i32* %op10
  %op35 = load i32, i32* %op8
  %op36 = add i32 %op35, 1
  store i32 %op36, i32* %op6
  br label %label_judgeBB2
label_outBB1:                                                ; preds = %label_judgeBB1
  %op37 = load i32, i32* %op5
  %op38 = sub i32 %op37, 1
  store i32 %op38, i32* %op6
  br label %label_judgeBB6
label_judgeBB2:                                                ; preds = %label_stateBB1, %label66
  %op39 = load i32, i32* %op6
  %op40 = load i32, i32* %op5
  %op41 = icmp slt i32 %op39, %op40
  %op42 = zext i1 %op41 to i32
  %op43 = icmp sgt i32 %op42, 0
  br i1 %op43, label %label_stateBB2, label %label_outBB2
label_stateBB2:                                                ; preds = %label_judgeBB2
  %op44 = load float*, float** %op4
  %op45 = load i32, i32* %op6
  %op46 = load i32, i32* %op11
  %op47 = load i32, i32* %op9
  %op48 = call float @get(float* %op44, i32 %op45, i32 %op46, i32 %op47)
  %op49 = call float @abs(float %op48)
  %op50 = load float*, float** %op4
  %op51 = load i32, i32* %op10
  %op52 = load i32, i32* %op11
  %op53 = load i32, i32* %op9
  %op54 = call float @get(float* %op50, i32 %op51, i32 %op52, i32 %op53)
  %op55 = call float @abs(float %op54)
  %op56 = fcmp ugt float %op49, %op55
  %op57 = zext i1 %op56 to i32
  %op58 = icmp ne i32 %op57, 0
  br i1 %op58, label %label64, label %label66
label_outBB2:                                                ; preds = %label_judgeBB2
  %op59 = load i32, i32* %op10
  %op60 = load i32, i32* %op8
  %op61 = icmp ne i32 %op59, %op60
  %op62 = zext i1 %op61 to i32
  %op63 = icmp ne i32 %op62, 0
  br i1 %op63, label %label69, label %label71
label64:                                                ; preds = %label_stateBB2
  %op65 = load i32, i32* %op6
  store i32 %op65, i32* %op10
  br label %label66
label66:                                                ; preds = %label_stateBB2, %label64
  %op67 = load i32, i32* %op6
  %op68 = add i32 %op67, 1
  store i32 %op68, i32* %op6
  br label %label_judgeBB2
label69:                                                ; preds = %label_outBB2
  %op70 = load i32, i32* %op8
  store i32 %op70, i32* %op7
  br label %label_judgeBB3
label71:                                                ; preds = %label_outBB2, %label_outBB3
  %op72 = load float*, float** %op4
  %op73 = load i32, i32* %op8
  %op74 = load i32, i32* %op11
  %op75 = load i32, i32* %op9
  %op76 = call float @get(float* %op72, i32 %op73, i32 %op74, i32 %op75)
  %op77 = call float @isZero(float %op76)
  %op78 = fcmp une float %op77, 0x0
  br i1 %op78, label %label117, label %label125
label_judgeBB3:                                                ; preds = %label69, %label111
  %op79 = load i32, i32* %op7
  %op80 = load i32, i32* %op9
  %op81 = icmp slt i32 %op79, %op80
  %op82 = zext i1 %op81 to i32
  %op83 = icmp sgt i32 %op82, 0
  br i1 %op83, label %label_stateBB3, label %label_outBB3
label_stateBB3:                                                ; preds = %label_judgeBB3
  %op84 = load float*, float** %op4
  %op85 = load i32, i32* %op8
  %op86 = load i32, i32* %op7
  %op87 = load i32, i32* %op9
  %op88 = call float @get(float* %op84, i32 %op85, i32 %op86, i32 %op87)
  store float %op88, float* %op12
  %op89 = load float*, float** %op4
  %op90 = load i32, i32* %op10
  %op91 = load i32, i32* %op7
  %op92 = load i32, i32* %op9
  %op93 = call float @get(float* %op89, i32 %op90, i32 %op91, i32 %op92)
  %op94 = load i32, i32* %op8
  %op95 = load i32, i32* %op9
  %op96 = mul i32 %op94, %op95
  %op97 = load i32, i32* %op7
  %op98 = add i32 %op96, %op97
  %op99 = icmp slt i32 %op98, 0
  br i1 %op99, label %label110, label %label100
label_outBB3:                                                ; preds = %label_judgeBB3
  br label %label71
label100:                                                ; preds = %label_stateBB3, %label110
  %op101 = load float*, float** %op4
  %op102 = getelementptr float, float* %op101, i32 %op98
  store float %op93, float* %op102
  %op103 = load float, float* %op12
  %op104 = load i32, i32* %op10
  %op105 = load i32, i32* %op9
  %op106 = mul i32 %op104, %op105
  %op107 = load i32, i32* %op7
  %op108 = add i32 %op106, %op107
  %op109 = icmp slt i32 %op108, 0
  br i1 %op109, label %label116, label %label111
label110:                                                ; preds = %label_stateBB3
  call void @neg_idx_except()
  br label %label100
label111:                                                ; preds = %label100, %label116
  %op112 = load float*, float** %op4
  %op113 = getelementptr float, float* %op112, i32 %op108
  store float %op103, float* %op113
  %op114 = load i32, i32* %op7
  %op115 = add i32 %op114, 1
  store i32 %op115, i32* %op7
  br label %label_judgeBB3
label116:                                                ; preds = %label100
  call void @neg_idx_except()
  br label %label111
label117:                                                ; preds = %label71
  %op118 = load i32, i32* %op8
  %op119 = sub i32 %op118, 1
  store i32 %op119, i32* %op8
  br label %label120
label120:                                                ; preds = %label117, %label_outBB4
  %op121 = load i32, i32* %op8
  %op122 = add i32 %op121, 1
  store i32 %op122, i32* %op8
  %op123 = load i32, i32* %op11
  %op124 = add i32 %op123, 1
  store i32 %op124, i32* %op11
  br label %label_judgeBB1
label125:                                                ; preds = %label71
  %op126 = load i32, i32* %op8
  %op127 = add i32 %op126, 1
  store i32 %op127, i32* %op6
  br label %label_judgeBB4
label_judgeBB4:                                                ; preds = %label125, %label155
  %op128 = load i32, i32* %op6
  %op129 = load i32, i32* %op5
  %op130 = icmp slt i32 %op128, %op129
  %op131 = zext i1 %op130 to i32
  %op132 = icmp sgt i32 %op131, 0
  br i1 %op132, label %label_stateBB4, label %label_outBB4
label_stateBB4:                                                ; preds = %label_judgeBB4
  %op133 = load float*, float** %op4
  %op134 = load i32, i32* %op6
  %op135 = load i32, i32* %op11
  %op136 = load i32, i32* %op9
  %op137 = call float @get(float* %op133, i32 %op134, i32 %op135, i32 %op136)
  %op138 = call float @isZero(float %op137)
  %op139 = sitofp i32 1 to float
  %op140 = fsub float %op139, %op138
  %op141 = fcmp une float %op140, 0x0
  br i1 %op141, label %label142, label %label155
label_outBB4:                                                ; preds = %label_judgeBB4
  br label %label120
label142:                                                ; preds = %label_stateBB4
  %op143 = load float*, float** %op4
  %op144 = load i32, i32* %op6
  %op145 = load i32, i32* %op11
  %op146 = load i32, i32* %op9
  %op147 = call float @get(float* %op143, i32 %op144, i32 %op145, i32 %op146)
  %op148 = load float*, float** %op4
  %op149 = load i32, i32* %op8
  %op150 = load i32, i32* %op11
  %op151 = load i32, i32* %op9
  %op152 = call float @get(float* %op148, i32 %op149, i32 %op150, i32 %op151)
  %op153 = fdiv float %op147, %op152
  store float %op153, float* %op12
  %op154 = load i32, i32* %op11
  store i32 %op154, i32* %op7
  br label %label_judgeBB5
label155:                                                ; preds = %label_stateBB4, %label_outBB5
  %op156 = load i32, i32* %op6
  %op157 = add i32 %op156, 1
  store i32 %op157, i32* %op6
  br label %label_judgeBB4
label_judgeBB5:                                                ; preds = %label142, %label188
  %op158 = load i32, i32* %op7
  %op159 = load i32, i32* %op9
  %op160 = icmp slt i32 %op158, %op159
  %op161 = zext i1 %op160 to i32
  %op162 = icmp sgt i32 %op161, 0
  br i1 %op162, label %label_stateBB5, label %label_outBB5
label_stateBB5:                                                ; preds = %label_judgeBB5
  %op163 = load i32, i32* %op6
  %op164 = load i32, i32* %op9
  %op165 = mul i32 %op163, %op164
  %op166 = load i32, i32* %op7
  %op167 = add i32 %op165, %op166
  %op168 = icmp slt i32 %op167, 0
  br i1 %op168, label %label187, label %label169
label_outBB5:                                                ; preds = %label_judgeBB5
  br label %label155
label169:                                                ; preds = %label_stateBB5, %label187
  %op170 = load float*, float** %op4
  %op171 = getelementptr float, float* %op170, i32 %op167
  %op172 = load float, float* %op171
  %op173 = load float*, float** %op4
  %op174 = load i32, i32* %op8
  %op175 = load i32, i32* %op7
  %op176 = load i32, i32* %op9
  %op177 = call float @get(float* %op173, i32 %op174, i32 %op175, i32 %op176)
  %op178 = load float, float* %op12
  %op179 = fmul float %op177, %op178
  %op180 = fsub float %op172, %op179
  %op181 = load i32, i32* %op6
  %op182 = load i32, i32* %op9
  %op183 = mul i32 %op181, %op182
  %op184 = load i32, i32* %op7
  %op185 = add i32 %op183, %op184
  %op186 = icmp slt i32 %op185, 0
  br i1 %op186, label %label193, label %label188
label187:                                                ; preds = %label_stateBB5
  call void @neg_idx_except()
  br label %label169
label188:                                                ; preds = %label169, %label193
  %op189 = load float*, float** %op4
  %op190 = getelementptr float, float* %op189, i32 %op185
  store float %op180, float* %op190
  %op191 = load i32, i32* %op7
  %op192 = add i32 %op191, 1
  store i32 %op192, i32* %op7
  br label %label_judgeBB5
label193:                                                ; preds = %label169
  call void @neg_idx_except()
  br label %label188
label_judgeBB6:                                                ; preds = %label_outBB1, %label247
  %op194 = load i32, i32* %op6
  %op195 = icmp sge i32 %op194, 0
  %op196 = zext i1 %op195 to i32
  %op197 = icmp sgt i32 %op196, 0
  br i1 %op197, label %label_stateBB6, label %label_outBB6
label_stateBB6:                                                ; preds = %label_judgeBB6
  %op198 = load float*, float** %op4
  %op199 = load i32, i32* %op6
  %op200 = load i32, i32* %op5
  %op201 = load i32, i32* %op9
  %op202 = call float @get(float* %op198, i32 %op199, i32 %op200, i32 %op201)
  store float %op202, float* %op12
  %op203 = load i32, i32* %op6
  %op204 = add i32 %op203, 1
  store i32 %op204, i32* %op7
  br label %label_judgeBB7
label_outBB6:                                                ; preds = %label_judgeBB6
  ret i32 0
label_judgeBB7:                                                ; preds = %label_stateBB6, %label237
  %op205 = load i32, i32* %op7
  %op206 = load i32, i32* %op5
  %op207 = icmp slt i32 %op205, %op206
  %op208 = zext i1 %op207 to i32
  %op209 = icmp sgt i32 %op208, 0
  br i1 %op209, label %label_stateBB7, label %label_outBB7
label_stateBB7:                                                ; preds = %label_judgeBB7
  %op210 = load float*, float** %op4
  %op211 = load i32, i32* %op6
  %op212 = load i32, i32* %op7
  %op213 = load i32, i32* %op9
  %op214 = call float @get(float* %op210, i32 %op211, i32 %op212, i32 %op213)
  %op215 = call float @isZero(float %op214)
  %op216 = sitofp i32 1 to float
  %op217 = fsub float %op216, %op215
  %op218 = fcmp une float %op217, 0x0
  br i1 %op218, label %label228, label %label237
label_outBB7:                                                ; preds = %label_judgeBB7
  %op219 = load float, float* %op12
  %op220 = load float*, float** %op4
  %op221 = load i32, i32* %op6
  %op222 = load i32, i32* %op6
  %op223 = load i32, i32* %op9
  %op224 = call float @get(float* %op220, i32 %op221, i32 %op222, i32 %op223)
  %op225 = fdiv float %op219, %op224
  %op226 = load i32, i32* %op6
  %op227 = icmp slt i32 %op226, 0
  br i1 %op227, label %label252, label %label247
label228:                                                ; preds = %label_stateBB7
  %op229 = load float, float* %op12
  %op230 = load float*, float** %op4
  %op231 = load i32, i32* %op6
  %op232 = load i32, i32* %op7
  %op233 = load i32, i32* %op9
  %op234 = call float @get(float* %op230, i32 %op231, i32 %op232, i32 %op233)
  %op235 = load i32, i32* %op7
  %op236 = icmp slt i32 %op235, 0
  br i1 %op236, label %label246, label %label240
label237:                                                ; preds = %label_stateBB7, %label240
  %op238 = load i32, i32* %op7
  %op239 = add i32 %op238, 1
  store i32 %op239, i32* %op7
  br label %label_judgeBB7
label240:                                                ; preds = %label228, %label246
  %op241 = load float*, float** %op3
  %op242 = getelementptr float, float* %op241, i32 %op235
  %op243 = load float, float* %op242
  %op244 = fmul float %op234, %op243
  %op245 = fsub float %op229, %op244
  store float %op245, float* %op12
  br label %label237
label246:                                                ; preds = %label228
  call void @neg_idx_except()
  br label %label240
label247:                                                ; preds = %label_outBB7, %label252
  %op248 = load float*, float** %op3
  %op249 = getelementptr float, float* %op248, i32 %op226
  store float %op225, float* %op249
  %op250 = load i32, i32* %op6
  %op251 = sub i32 %op250, 1
  store i32 %op251, i32* %op6
  br label %label_judgeBB6
label252:                                                ; preds = %label_outBB7
  call void @neg_idx_except()
  br label %label247
}
define void @main() {
label_entry:
  %op0 = alloca i32
  %op1 = alloca [3 x float]
  %op2 = alloca [12 x float]
  %op3 = icmp slt i32 0, 0
  br i1 %op3, label %label8, label %label4
label4:                                                ; preds = %label_entry, %label8
  %op5 = getelementptr [12 x float], [12 x float]* %op2, i32 0, i32 0
  %op6 = sitofp i32 1 to float
  store float %op6, float* %op5
  %op7 = icmp slt i32 1, 0
  br i1 %op7, label %label13, label %label9
label8:                                                ; preds = %label_entry
  call void @neg_idx_except()
  br label %label4
label9:                                                ; preds = %label4, %label13
  %op10 = getelementptr [12 x float], [12 x float]* %op2, i32 0, i32 1
  %op11 = sitofp i32 2 to float
  store float %op11, float* %op10
  %op12 = icmp slt i32 2, 0
  br i1 %op12, label %label18, label %label14
label13:                                                ; preds = %label4
  call void @neg_idx_except()
  br label %label9
label14:                                                ; preds = %label9, %label18
  %op15 = getelementptr [12 x float], [12 x float]* %op2, i32 0, i32 2
  %op16 = sitofp i32 1 to float
  store float %op16, float* %op15
  %op17 = icmp slt i32 3, 0
  br i1 %op17, label %label25, label %label19
label18:                                                ; preds = %label9
  call void @neg_idx_except()
  br label %label14
label19:                                                ; preds = %label14, %label25
  %op20 = getelementptr [12 x float], [12 x float]* %op2, i32 0, i32 3
  %op21 = sitofp i32 1 to float
  store float %op21, float* %op20
  %op22 = mul i32 1, 4
  %op23 = add i32 %op22, 0
  %op24 = icmp slt i32 %op23, 0
  br i1 %op24, label %label32, label %label26
label25:                                                ; preds = %label14
  call void @neg_idx_except()
  br label %label19
label26:                                                ; preds = %label19, %label32
  %op27 = getelementptr [12 x float], [12 x float]* %op2, i32 0, i32 %op23
  %op28 = sitofp i32 2 to float
  store float %op28, float* %op27
  %op29 = mul i32 1, 4
  %op30 = add i32 %op29, 1
  %op31 = icmp slt i32 %op30, 0
  br i1 %op31, label %label39, label %label33
label32:                                                ; preds = %label19
  call void @neg_idx_except()
  br label %label26
label33:                                                ; preds = %label26, %label39
  %op34 = getelementptr [12 x float], [12 x float]* %op2, i32 0, i32 %op30
  %op35 = sitofp i32 3 to float
  store float %op35, float* %op34
  %op36 = mul i32 1, 4
  %op37 = add i32 %op36, 2
  %op38 = icmp slt i32 %op37, 0
  br i1 %op38, label %label46, label %label40
label39:                                                ; preds = %label26
  call void @neg_idx_except()
  br label %label33
label40:                                                ; preds = %label33, %label46
  %op41 = getelementptr [12 x float], [12 x float]* %op2, i32 0, i32 %op37
  %op42 = sitofp i32 4 to float
  store float %op42, float* %op41
  %op43 = mul i32 1, 4
  %op44 = add i32 %op43, 3
  %op45 = icmp slt i32 %op44, 0
  br i1 %op45, label %label53, label %label47
label46:                                                ; preds = %label33
  call void @neg_idx_except()
  br label %label40
label47:                                                ; preds = %label40, %label53
  %op48 = getelementptr [12 x float], [12 x float]* %op2, i32 0, i32 %op44
  %op49 = sitofp i32 3 to float
  store float %op49, float* %op48
  %op50 = mul i32 2, 4
  %op51 = add i32 %op50, 0
  %op52 = icmp slt i32 %op51, 0
  br i1 %op52, label %label60, label %label54
label53:                                                ; preds = %label40
  call void @neg_idx_except()
  br label %label47
label54:                                                ; preds = %label47, %label60
  %op55 = getelementptr [12 x float], [12 x float]* %op2, i32 0, i32 %op51
  %op56 = sitofp i32 1 to float
  store float %op56, float* %op55
  %op57 = mul i32 2, 4
  %op58 = add i32 %op57, 1
  %op59 = icmp slt i32 %op58, 0
  br i1 %op59, label %label68, label %label61
label60:                                                ; preds = %label47
  call void @neg_idx_except()
  br label %label54
label61:                                                ; preds = %label54, %label68
  %op62 = getelementptr [12 x float], [12 x float]* %op2, i32 0, i32 %op58
  %op63 = sitofp i32 1 to float
  store float %op63, float* %op62
  %op64 = sub i32 0, 2
  %op65 = mul i32 2, 4
  %op66 = add i32 %op65, 2
  %op67 = icmp slt i32 %op66, 0
  br i1 %op67, label %label75, label %label69
label68:                                                ; preds = %label54
  call void @neg_idx_except()
  br label %label61
label69:                                                ; preds = %label61, %label75
  %op70 = getelementptr [12 x float], [12 x float]* %op2, i32 0, i32 %op66
  %op71 = sitofp i32 %op64 to float
  store float %op71, float* %op70
  %op72 = mul i32 2, 4
  %op73 = add i32 %op72, 3
  %op74 = icmp slt i32 %op73, 0
  br i1 %op74, label %label82, label %label76
label75:                                                ; preds = %label61
  call void @neg_idx_except()
  br label %label69
label76:                                                ; preds = %label69, %label82
  %op77 = getelementptr [12 x float], [12 x float]* %op2, i32 0, i32 %op73
  %op78 = sitofp i32 0 to float
  store float %op78, float* %op77
  %op79 = getelementptr [3 x float], [3 x float]* %op1, i32 0, i32 0
  %op80 = getelementptr [12 x float], [12 x float]* %op2, i32 0, i32 0
  %op81 = call i32 @gauss(float* %op79, float* %op80, i32 3)
  store i32 0, i32* %op0
  br label %label_judgeBB8
label82:                                                ; preds = %label69
  call void @neg_idx_except()
  br label %label76
label_judgeBB8:                                                ; preds = %label76, %label89
  %op83 = load i32, i32* %op0
  %op84 = icmp slt i32 %op83, 3
  %op85 = zext i1 %op84 to i32
  %op86 = icmp sgt i32 %op85, 0
  br i1 %op86, label %label_stateBB8, label %label_outBB8
label_stateBB8:                                                ; preds = %label_judgeBB8
  %op87 = load i32, i32* %op0
  %op88 = icmp slt i32 %op87, 0
  br i1 %op88, label %label94, label %label89
label_outBB8:                                                ; preds = %label_judgeBB8
  ret void
label89:                                                ; preds = %label_stateBB8, %label94
  %op90 = getelementptr [3 x float], [3 x float]* %op1, i32 0, i32 %op87
  %op91 = load float, float* %op90
  call void @outputFloat(float %op91)
  %op92 = load i32, i32* %op0
  %op93 = add i32 %op92, 1
  store i32 %op93, i32* %op0
  br label %label_judgeBB8
label94:                                                ; preds = %label_stateBB8
  call void @neg_idx_except()
  br label %label89
}
