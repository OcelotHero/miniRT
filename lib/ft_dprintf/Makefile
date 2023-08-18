############################### Files and directories ###############################
# Common source files
SRC_PAR	= parse
SRC_PRN = char nb_base none null percent ptr str
SRC_UTL = bzero calloc contains isdigit pos_atoi strlen
SRC_DEF = dprintf

# Mandatory source files
SRC_MAN	= parse_config_m

# Bonus source files
SRC_BNS	= parse_config_b

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Subdirectories
PRNTR_D = printer
UTILS_D = utils

PARSR_D = parser
MAN_DIR = mandatory
BNS_DIR = bonus

##############################        Objects        ################################
OBJS	+= $(addprefix $(OBJ_DIR)/ft_, $(addsuffix .o, $(SRC_UTL)))
OBJS	+= $(addprefix $(OBJ_DIR)/ft_, $(addsuffix .o, $(SRC_PAR)))
OBJS	+= $(addprefix $(OBJ_DIR)/ft_, $(addsuffix .o, $(SRC_DEF)))
OBJS	+= $(addprefix $(OBJ_DIR)/ft_dprint_, $(addsuffix .o, $(SRC_PRN)))

OBJS_M	+= $(addprefix $(OBJ_DIR)/ft_, $(addsuffix .o, $(SRC_MAN)))

OBJS_B	+= $(addprefix $(OBJ_DIR)/ft_, $(addsuffix .o, $(SRC_BNS)))

##############################  Config declarations  #################################
NAME	= libftdprintf.a
INCL	= inc

VPATH	+= %.c $(SRC_DIR)
VPATH	+= %.c $(addprefix $(SRC_DIR)/, $(PRNTR_D) $(UTILS_D) $(PARSR_D) \
				 $(PARSR_D)/$(MAN_DIR) $(PARSR_D)/$(BNS_DIR))

LIB		= ar rc
CC		= cc
FLAGS	= -Wall -Wextra -Werror -g
RM		= rm -rf

all:		${NAME}

${NAME}:	${OBJS} ${OBJS_M}
			@echo "    ${NAME}"
			@${RM} ${OBJ_DIR}/ft_${SRC_BNS}.o
			@${RM} ${NAME}
			@${LIB} ${NAME} ${OBJS} ${OBJS_M}

${OBJ_DIR}/%.o: %.c | ${OBJ_DIR}
			@echo "    $<"
			@${CC} ${FLAGS} -c $< -o $@ -I ${INCL}

${OBJ_DIR}:
			@mkdir -p ${OBJ_DIR}

bonus:		${OBJS} ${OBJS_B}
			@echo "    ${NAME}"
			@${RM} ${OBJ_DIR}/ft_${SRC_MAN}.o
			@${RM} ${NAME}
			@${LIB} ${NAME} ${OBJS} ${OBJS_B}

clean:
			${RM} ${OBJ_DIR}

fclean:		clean
			${RM} ${NAME}

re:			fclean all

.PHONY:		all clean fclean re
