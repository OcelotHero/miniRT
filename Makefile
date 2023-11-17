############################### Files and directories ###############################
# Common source files
SRC_MAT = vec3 vec4
SRC_UTL = utils

# Mandatory source files
SRC_P_M = parser_m
SRC_R_M	= renderer intersection
SRC_MAN = minirt

# Bonus source files
SRC_P_B = parser_b get_values parser_json json_utils init_vec3 init_vec4
SRC_C_B = key_events loop_events mouse_events window_events
SRC_R_B	= opengl texture
SRC_U_B	= utils_b
SRC_BNS =

# Conditional recompilation source files
SRC_REC = parser_c

# Directories
SRC_DIR = src
LIB_DIR = lib
OBJ_DIR = obj

# Subdirectories
PARSR_D = parser
MATHS_D = maths
UTILS_D = utils

OPNGL_D = opengl
CBACK_D = callback
RENDR_D = renderer
MAN_DIR = mandatory
BNS_DIR = bonus

# libft
LIBFT_D = libft
LIBFT_N = ft
LIBFT_L	= $(addprefix $(LIB_DIR)/${LIBFT_D}/lib, $(addsuffix .a, $(LIBFT_N)))

# ft_dprintf
FPRNF_D = ft_dprintf
FPRNF_N = ftdprintf
FPRNF_L	= $(addprefix $(LIB_DIR)/${FPRNF_D}/lib, $(addsuffix .a, $(FPRNF_N)))

# MLX42
MLX42_D = MLX42
MLX42_N = mlx42
MLX42_L	= $(addprefix $(LIB_DIR)/${MLX42_D}/lib, $(addsuffix .a, $(MLX42_N)))

# cJSON
CJSON_D = cJSON
CJSON_N = cjson
CJSON_L	= $(addprefix $(LIB_DIR)/${CJSON_D}/lib, $(addsuffix .a, $(CJSON_N)))

##############################        Objects        ################################
OBJS	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_MAT)))
OBJS	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_UTL)))

OBJS_R	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_REC)))

OBJS_M	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_P_M)))
OBJS_M	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_R_M)))
OBJS_M	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_MAN)))

OBJS_B	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_P_B)))
OBJS_B	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_C_B)))
OBJS_B	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_R_B)))
OBJS_B	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_U_B)))
OBJS_B	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_BNS)))

##############################  Config declarations  #################################
NAME_M	= minirt
NAME_B	= minirt_bonus
INCL	= inc

VPATH	+= %.c $(SRC_DIR)
VPATH	+= %.c $(addprefix $(SRC_DIR)/, $(MATHS_D) $(UTILS_D) $(PARSR_D) $(CBACK_D)\
				  $(PARSR_D)/$(MAN_DIR) $(PARSR_D)/$(BNS_DIR) $(UTILS_D)/$(BNS_DIR)\
				  $(RENDR_D)/$(MAN_DIR) $(RENDR_D)/$(BNS_DIR))

CC		= cc
FLAGS	= -g -O0 -MMD -MP #-Wall -Wextra -Werror
RM		= rm -rf

OSNAME	= $(shell uname -s)

ifeq ($(shell test -d "/Users/${USER}/.brew"; echo $$?), 1)
	BREW_D = homebrew
else
	BREW_D = .brew
endif

OPTS	+= -pthread -lglfw -ldl -lm
ifeq (${OSNAME}, Darwin)
	OPTS	+= -L"/Users/${USER}/${BREW_D}/Cellar/glfw/3.3.8/lib/"
	OPTS	+= -framework Cocoa -framework OpenGL -framework IOKit
else
	OPTS	+= -lGL
endif

all:		${NAME_M}

${NAME_M}:	FLAGS += -D BNS=0 -I ${INCL}/${MAN_DIR}
${NAME_M}:	${LIBFT_L} ${FPRNF_L} ${MLX42_L} ${OBJS} ${OBJS_R} ${OBJS_M}
			@if [ -f ${NAME_B} ]; then ${RM} ${OBJS_R}; make ${OBJS_R}; fi
			@echo "    ${NAME_M}"
			@${CC} ${FLAGS} ${OBJS} ${OBJS_R} ${OBJS_M} ${MLX42_L} ${LIBFT_L} \
				${FPRNF_L} -o ${NAME_M} ${OPTS}

${OBJ_DIR}/%.o: %.c | ${OBJ_DIR}
			@echo "    $<"
			@${CC} ${FLAGS} -c $< -o $@ -I ${INCL} -I ${LIB_DIR}/${FPRNF_D}/${INCL} \
				-I ${LIB_DIR}/${LIBFT_D}/${INCL} -I ${LIB_DIR}/${MLX42_D}/include \
				-I ${LIB_DIR}/${CJSON_D} -I lib/stb_image

${OBJ_DIR}:
			@mkdir -p ${OBJ_DIR}

${LIBFT_L}:
			@echo "    $@"
			@make -C ${LIB_DIR}/${LIBFT_D}

${FPRNF_L}:
			@echo "    $@"
			@make -C ${LIB_DIR}/${FPRNF_D} bonus

${MLX42_L}:
			@echo "    $@"
			@make -C ${LIB_DIR}/${MLX42_D}

${CJSON_L}:
			@echo "    $@"
			@make static -C ${LIB_DIR}/${CJSON_D}

clean:
			@make -C ${LIB_DIR}/${FPRNF_D} clean
			@make -C ${LIB_DIR}/${LIBFT_D} clean
			@make -C ${LIB_DIR}/${MLX42_D} clean
			@make -C ${LIB_DIR}/${CJSON_D} clean
			${RM} ${OBJ_DIR}

fclean:		clean
			@make -C ${LIB_DIR}/${FPRNF_D} fclean
			@make -C ${LIB_DIR}/${LIBFT_D} fclean
			@make -C ${LIB_DIR}/${MLX42_D} fclean
			@make -C ${LIB_DIR}/${CJSON_D} clean
			${RM} ${NAME_M} ${NAME_B}

bonus:		${NAME_B}

${NAME_B}:	FLAGS += -D BNS=1 -I ${INCL}/${BNS_DIR}
${NAME_B}:	${LIBFT_L} ${FPRNF_L} ${MLX42_L} ${CJSON_L} ${OBJS} ${OBJS_R} ${OBJS_B}
			@if [ -f ${NAME_M} ]; then ${RM} ${OBJS_R}; make ${OBJS_R}; fi
			@echo "    ${NAME_B}"
			@${CC} ${FLAGS} ${OBJS} ${OBJS_R} ${OBJS_B} ${MLX42_L} ${LIBFT_L} \
				${FPRNF_L} ${CJSON_L} -o ${NAME_B} ${OPTS}

re:			fclean all

.PHONY:		all clean fclean re

-include $(OBJS:.o=.d)
