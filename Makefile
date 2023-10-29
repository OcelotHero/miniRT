############################### Files and directories ###############################
# Common source files
SRC_PAR	= parser_c
SRC_MAT = vec3 vec4
SRC_UTL = utils

# Mandatory source files
SRC_P_M = parser_m
SRC_C_M =
SRC_R_M	= renderer intersection
SRC_MAN = minirt

# Bonus source files
SRC_P_B = parser_b
SRC_C_B = key_events loop_events mouse_events window_events
SRC_R_B	=
SRC_BNS = opengl

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

##############################        Objects        ################################
OBJS	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_PAR)))
OBJS	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_MAT)))
OBJS_B	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_UTL)))

OBJS_M	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_P_M)))
OBJS_M	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_C_M)))
OBJS_M	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_R_M)))
OBJS_M	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_MAN)))

OBJS_B	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_P_B)))
OBJS_B	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_C_B)))
OBJS_B	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_R_B)))
OBJS_B	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_BNS)))

##############################  Config declarations  #################################
NAME_M	= minirt
NAME_B	= minirt_bonus
INCL	= inc

VPATH	+= %.c $(SRC_DIR)
VPATH	+= %.c $(addprefix $(SRC_DIR)/, $(MATHS_D) $(UTILS_D) $(RENDR_D) $(CBACK_D)\
				  $(OPNGL_D) $(PARSR_D) $(PARSR_D)/$(MAN_DIR) $(PARSR_D)/$(BNS_DIR))

CC		= cc
FLAGS	= -g -O0 #-Wall -Wextra -Werror
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
${NAME_M}:	${LIBFT_L} ${FPRNF_L} ${MLX42_L} ${OBJS} ${OBJS_M}
			@${RM} ${OBJS_B}
			@echo "    ${NAME_M}"
			@${CC} ${FLAGS} ${OBJS} ${OBJS_M} ${MLX42_L} ${LIBFT_L} ${FPRNF_L} -o ${NAME_M} ${OPTS}

${OBJ_DIR}/%.o: %.c | ${OBJ_DIR}
			@echo "    $<"
			@${CC} ${FLAGS} -c $< -o $@ -I ${INCL} -I ${LIB_DIR}/${FPRNF_D}/${INCL} \
				-I ${LIB_DIR}/${LIBFT_D}/${INCL} -I ${LIB_DIR}/${MLX42_D}/include \
				-I lib/stb_image

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

clean:
			@make -C ${LIB_DIR}/${FPRNF_D} clean
			@make -C ${LIB_DIR}/${LIBFT_D} clean
			@make -C ${LIB_DIR}/${MLX42_D} clean
			${RM} ${OBJ_DIR}

fclean:		clean
			@make -C ${LIB_DIR}/${FPRNF_D} fclean
			@make -C ${LIB_DIR}/${LIBFT_D} fclean
			@make -C ${LIB_DIR}/${MLX42_D} fclean
			${RM} ${NAME_M} ${NAME_B}

bonus:		FLAGS += -D BNS=1 -I ${INCL}/${BNS_DIR}
bonus:		${NAME_B}

${NAME_B}:	${LIBFT_L} ${FPRNF_L} ${MLX42_L} ${OBJS} ${OBJS_B}
			@${RM} ${OBJS_M}
			@echo "    ${NAME_B}"
			@${CC} ${FLAGS} ${OBJS} ${OBJS_B} ${MLX42_L} ${LIBFT_L} ${FPRNF_L} -o ${NAME_B} ${OPTS}

re:			fclean all

.PHONY:		all clean fclean re
