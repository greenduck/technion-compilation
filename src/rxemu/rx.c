#include <stdio.h>
#include <string.h>

#define AsmSize 1000 /* Limit on number of lines. */

#define RegFileSize 999 /* Number of registers. */

#define MemSize 9999 /* Limit on number of memory entries. */

typedef enum {IASN, IPRT, IINP, IEQL, INQL, ILSS, IGRT, IADD, ISUB, IMLT, IDIV, ILOD, ISTR, RASN, RPRT, RINP, REQL, RNQL, RLSS, RGRT, RADD, RSUB, RMLT, RDIV, RLOD, RSTR, ITOR, RTOI, JUMP, JMPZ, JMPNZ, JLINK, RETRN, HALT} Opcode_t;

/* Data structure holding the program.  */
/* For each instruction we hold its Opcode and (upto) 3 operands.
 * We hold 3 fields for each operand:
 *   1. an integer field (Arg1,Arg2,Arg3);
 *   2. a float field (Arg1_real_const, etc.);
 *   3. an indicator field (Arg1_is_const, etc.).
 * When operand x represents an integer or real register,
 *  Argx holds the register number, Argx_is_const is 0.
 * When operand x represents an integer constant (a value loaded into
 *  an integer register, a memory address, a label to jump to),
 *  Argx holds the value of this integer constant, and Argx_is_const is 1.
 * When operand x represents an real constant, Argx_real_const holds this
 *  value, and Argx_is_const is 1. */

Opcode_t Opcode [AsmSize];
int Arg1 [AsmSize];
int Arg1_is_const [AsmSize];
float Arg1_real_const [AsmSize];
int Arg2 [AsmSize];
int Arg2_is_const [AsmSize];
float Arg2_real_const [AsmSize];
int Arg3 [AsmSize];
int Arg3_is_const [AsmSize];
float Arg3_real_const [AsmSize];

/* Data structure holding runtime environment (regs and mem).  */
int IREG [RegFileSize];
int IMEM [MemSize];
float RREG [RegFileSize];
float RMEM [MemSize];

int get_reg(FILE* fp, char prefix)
{
  char regtype [2];
  int regnum = -1;

  if (fscanf (fp, "%1s", regtype) != 1)
    printf ("Error: regtype scanf failed.\n");
  else if (regtype[0] != prefix)
    printf ("Error: illegal regtype prefix %s\n", regtype);
  else if (fscanf (fp, "%d", &regnum) != 1)
    printf ("Error: regnum scanf failed.\n");
  else if ((regnum < 0) || (regnum > RegFileSize))
    printf ("Error: regnum %d out of bounds.\n", regnum);

  return regnum;
}

int get_ireg_or_iconst(FILE* fp, int* is_const)
{
  int result = -1;

  if (fscanf (fp, "%d", &result) == 1)
    {
#ifdef DEBUG
      printf ("Debug: read iconst %d\n", result);
#endif
      *is_const = 1;
      return result;
    }

  *is_const = 0;
  return get_reg(fp, 'I');
}

int get_iconst(FILE* fp)
{
  int result = -1;

  if (fscanf (fp, "%d", &result) != 1)
    {
      printf ("Error: failed trying to read an iconst.\n");
      return -1;
    }

#ifdef DEBUG
  printf ("Debug: read iconst %d\n", result);
#endif

  return result;
}

/* if the argument is a floating-point constant,
 * sets is_reg to 0 and returns this constant.
 * if the argument is a Real register, sets is_reg
 * to 1 and returns the number of the register (>=0). */
float get_rreg_or_rconst(FILE* fp, int* is_reg)
{
  float result = -1.0;

  if (fscanf (fp, "%f", &result) == 1)
    {
#ifdef DEBUG
      printf ("Debug: read fconst %f\n", result);
#endif
      *is_reg = 0;
      return result;
    }

  *is_reg = 1;
  result = get_reg(fp, 'R');
  return result;
}

int read_AorDBC (FILE* fp, int line_no, char prefix)
{
  int is_const;

  Arg1[line_no] = get_reg(fp, prefix);
  Arg2[line_no] = get_ireg_or_iconst(fp, &is_const);
  Arg2_is_const[line_no] = is_const;
  Arg3[line_no] = get_ireg_or_iconst(fp, &is_const);
  Arg3_is_const[line_no] = is_const;
  if (Arg1[line_no] == -1 || Arg2[line_no] == -1 ||
      Arg3[line_no] == -1)
    return 1;
  return 0;
}

int read_AorDEF (FILE* fp, int line_no, char prefix)
{
  int rreg;
  float rconst;

  Arg1[line_no] = get_reg(fp, prefix);

  rconst = get_rreg_or_rconst(fp, &rreg);
  Arg2_is_const[line_no] = (rreg == 0);
  Arg2[line_no] = (int)rconst;
  if (rreg == 0)
    Arg2_real_const[line_no] = rconst;

  rconst = get_rreg_or_rconst(fp, &rreg);
  Arg3_is_const[line_no] = (rreg == 0);
  Arg3[line_no] = (int)rconst;
  if (rreg == 0)
    Arg3_real_const[line_no] = rconst;

  if (Arg1[line_no] == -1 || Arg2[line_no] == -1 ||
      Arg3[line_no] == -1)
    return 1;
  return 0;
}

int load_program (char* file_name)
{
  FILE* fp;
  char opcode [6];
  int line_no = 0;
  int error = 0;

  if ((fp = fopen(file_name, "r")) == NULL)
    {
      printf ("cannot open file %s\n", file_name);
      return -1;
    }

  while (fscanf (fp, "%5s", opcode) == 1)
    {
      line_no++;
#ifdef DEBUG
      printf ("Debug: line_no %d\n", line_no);
      printf ("Debug: read opcode %s\n", opcode);
#endif
      if (strncmp (opcode, "COPYI", 5) == 0)
        {
          int is_const;

          Opcode[line_no] = IASN;
          Arg1[line_no] = get_reg(fp, 'I');
          Arg2[line_no] = get_ireg_or_iconst(fp, &is_const);
          Arg2_is_const[line_no] = is_const;
          if (Arg1[line_no] == -1 || Arg2[line_no] == -1)
            error = 1;
        }
      else if (strncmp (opcode, "PRNTI", 5) == 0)
        {
          int is_const;

          Opcode[line_no] = IPRT;
          Arg1[line_no] = get_ireg_or_iconst(fp, &is_const);
          Arg1_is_const[line_no] = is_const;
          if (Arg1[line_no] == -1)
            error = 1;
        }
      else if (strncmp (opcode, "READI", 5) == 0)
        {
          Opcode[line_no] = IINP;
          Arg1[line_no] = get_reg(fp, 'I');
          if (Arg1[line_no] == -1)
            error = 1;
        }
      else if (strncmp (opcode, "SEQUI", 5) == 0)
        {
          Opcode[line_no] = IEQL;
          error = read_AorDBC (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "SNEQI", 5) == 0)
        {
          Opcode[line_no] = INQL;
          error = read_AorDBC (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "SLETI", 5) == 0)
        {
          Opcode[line_no] = ILSS;
          error = read_AorDBC (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "SGRTI", 5) == 0)
        {
          Opcode[line_no] = IGRT;
          error = read_AorDBC (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "ADD2I", 5) == 0)
        {
          Opcode[line_no] = IADD;
          error = read_AorDBC (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "SUBTI", 5) == 0)
        {
          Opcode[line_no] = ISUB;
          error = read_AorDBC (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "MULTI", 5) == 0)
        {
          Opcode[line_no] = IMLT;
          error = read_AorDBC (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "DIVDI", 5) == 0)
        {
          Opcode[line_no] = IDIV;
          error = read_AorDBC (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "LOADI", 5) == 0)
        {
          Opcode[line_no] = ILOD;
          error = read_AorDBC (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "STORI", 5) == 0)
        {
          Opcode[line_no] = ISTR;
          error = read_AorDBC (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "COPYR", 5) == 0)
        {
          int rreg;
          float rconst;

          Opcode[line_no] = RASN;
          Arg1[line_no] = get_reg(fp, 'R');
          rconst = get_rreg_or_rconst(fp, &rreg);
          Arg2_is_const[line_no] = (rreg == 0);
          Arg2[line_no] = (int)rconst;
          if (rreg == 0)
            Arg2_real_const[line_no] = rconst;
          if (Arg1[line_no] == -1 || Arg2[line_no] == -1)
            error = 1;
        }
      else if (strncmp (opcode, "PRNTR", 5) == 0)
        {
          int rreg;
          float rconst;

          Opcode[line_no] = RPRT;
          rconst = get_rreg_or_rconst(fp, &rreg);
          Arg1_is_const[line_no] = (rreg == 0);
          Arg1[line_no] = (int)rconst;
          if (rreg == 0)
            Arg1_real_const[line_no] = rconst;
          if (Arg1[line_no] == -1)
            error = 1;
        }
      else if (strncmp (opcode, "READR", 5) == 0)
        {
          Opcode[line_no] = RINP;
          Arg1[line_no] = get_reg(fp, 'R');
          if (Arg1[line_no] == -1)
            error = 1;
        }
      else if (strncmp (opcode, "SEQUR", 5) == 0)
        {
          Opcode[line_no] = REQL;
          error = read_AorDEF (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "SNEQR", 5) == 0)
        {
          Opcode[line_no] = RNQL;
          error = read_AorDEF (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "SLETR", 5) == 0)
        {
          Opcode[line_no] = RLSS;
          error = read_AorDEF (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "SGRTR", 5) == 0)
        {
          Opcode[line_no] = RGRT;
          error = read_AorDEF (fp, line_no, 'I');
        }
      else if (strncmp (opcode, "ADD2R", 5) == 0)
        {
          Opcode[line_no] = RADD;
          error = read_AorDEF (fp, line_no, 'R');
        }
      else if (strncmp (opcode, "SUBTR", 5) == 0)
        {
          Opcode[line_no] = RSUB;
          error = read_AorDEF (fp, line_no, 'R');
        }
      else if (strncmp (opcode, "MULTR", 5) == 0)
        {
          Opcode[line_no] = RMLT;
          error = read_AorDEF (fp, line_no, 'R');
        }
      else if (strncmp (opcode, "DIVDR", 5) == 0)
        {
          Opcode[line_no] = RDIV;
          error = read_AorDEF (fp, line_no, 'R');
        }
      else if (strncmp (opcode, "LOADR", 5) == 0)
        {
          Opcode[line_no] = RLOD;
          error = read_AorDBC (fp, line_no, 'R');
        }
      else if (strncmp (opcode, "STORR", 5) == 0)
        {
          Opcode[line_no] = RSTR;
          error = read_AorDBC (fp, line_no, 'R');
        }
      else if (strncmp (opcode, "CITOR", 5) == 0)
        {
          int is_const;

          Opcode[line_no] = ITOR;
          Arg1[line_no] = get_reg(fp, 'R');
          Arg2[line_no] = get_ireg_or_iconst(fp, &is_const);
          Arg2_is_const[line_no] = is_const;
          if (Arg1[line_no] == -1 || Arg2[line_no] == -1)
            error = 1;
        }
      else if (strncmp (opcode, "CRTOI", 5) == 0)
        {
          int rreg;
          float rconst;

          Opcode[line_no] = RTOI;
          Arg1[line_no] = get_reg(fp, 'I');
          rconst = get_rreg_or_rconst(fp, &rreg);
          Arg2_is_const[line_no] = (rreg == 0);
          Arg2[line_no] = (int)rconst;
          if (rreg == 0)
            Arg2_real_const[line_no] = rconst;
          if (Arg1[line_no] == -1 || Arg2[line_no] == -1)
            error = 1;
        }
      else if (strncmp (opcode, "UJUMP", 5) == 0)
        {
          Opcode[line_no] = JUMP;
          Arg1[line_no] = get_iconst(fp);
          if (Arg1[line_no] == -1)
            error = 1;
        }
      else if (strncmp (opcode, "JLINK", 5) == 0)
        {
          Opcode[line_no] = JLINK;
          Arg1[line_no] = get_iconst(fp);
          if (Arg1[line_no] == -1)
            error = 1;
        }
      else if (strncmp (opcode, "BREQZ", 5) == 0)
        {
          int is_const;

          Opcode[line_no] = JMPZ;
          Arg1[line_no] = get_ireg_or_iconst(fp, &is_const);
          Arg2[line_no] = get_iconst(fp);
          Arg1_is_const[line_no] = is_const;
          if (Arg1[line_no] == -1 || Arg2[line_no] == -1)
            error = 1;
        }
      else if (strncmp (opcode, "BNEQZ", 5) == 0)
        {
          int is_const;

          Opcode[line_no] = JMPNZ;
          Arg1[line_no] = get_ireg_or_iconst(fp, &is_const);
          Arg2[line_no] = get_iconst(fp);
          Arg1_is_const[line_no] = is_const;
          if (Arg1[line_no] == -1 || Arg2[line_no] == -1)
            error = 1;
        }
      else if (strncmp (opcode, "RETRN", 5) == 0)
        {
          Opcode[line_no] = RETRN;
        }
      else if (strncmp (opcode, "HALT", 4) == 0)
        {
          Opcode[line_no] = HALT;
        }
      else
        {
          printf ("Error: unrecognized opcode %s, bailing out.\n", opcode);
          error = 1;
        }

      if (error == 1)
        break;
    }

#ifdef DEBUG
  printf ("Debug: exiting load_program with");
  if (error == 0)
    printf ("out any error. ");
  else
    printf (" an error. ");
  printf ("Loaded %d lines.\n", line_no);
#endif

  fclose (fp);

  return (error == 0 ? line_no : -1);
}

int execute_line (int line)
{
  int next_line = line + 1;
  int arg1, arg2, arg3;
  float arg2_f, arg3_f;

#ifdef DEBUG
  printf ("Debug: executing line %d.\n", line);
#endif

  switch (Opcode[line])
    {
      case IASN:
        if (Arg2_is_const[line])
          IREG[Arg1[line]] = Arg2[line];
        else
          IREG[Arg1[line]] = IREG[Arg2[line]];
        break;
      case IPRT:
        if (Arg1_is_const[line])
          printf ("%d ", Arg1[line]);
        else
          printf ("%d ", IREG[Arg1[line]]);
        break;
      case IINP:
        printf ("Input integer?:");
        scanf ("%d", &arg2);
        IREG[Arg1[line]] = arg2;
        break;
      case IEQL:
        arg2 = (Arg2_is_const[line] ? Arg2[line]
                : IREG[Arg2[line]]);
        arg3 = (Arg3_is_const[line] ? Arg3[line]
                : IREG[Arg3[line]]);
        IREG[Arg1[line]] = (arg2 == arg3 ? 1 : 0);
        break;
      case INQL:
        arg2 = (Arg2_is_const[line] ? Arg2[line]
                : IREG[Arg2[line]]);
        arg3 = (Arg3_is_const[line] ? Arg3[line]
                : IREG[Arg3[line]]);
        IREG[Arg1[line]] = (arg2 != arg3 ? 1 : 0);
        break;
      case ILSS:
        arg2 = (Arg2_is_const[line] ? Arg2[line]
                : IREG[Arg2[line]]);
        arg3 = (Arg3_is_const[line] ? Arg3[line]
                : IREG[Arg3[line]]);
        IREG[Arg1[line]] = (arg2 < arg3 ? 1 : 0);
        break;
      case IGRT:
        arg2 = (Arg2_is_const[line] ? Arg2[line]
                : IREG[Arg2[line]]);
        arg3 = (Arg3_is_const[line] ? Arg3[line]
                : IREG[Arg3[line]]);
        IREG[Arg1[line]] = (arg2 > arg3 ? 1 : 0);
        break;
      case IADD:
        arg2 = (Arg2_is_const[line] ? Arg2[line]
                : IREG[Arg2[line]]);
        arg3 = (Arg3_is_const[line] ? Arg3[line]
                : IREG[Arg3[line]]);
        IREG[Arg1[line]] = arg2 + arg3;
        break;
      case ISUB:
        arg2 = (Arg2_is_const[line] ? Arg2[line]
                : IREG[Arg2[line]]);
        arg3 = (Arg3_is_const[line] ? Arg3[line]
                : IREG[Arg3[line]]);
        IREG[Arg1[line]] = arg2 - arg3;
        break;
      case IMLT:
        arg2 = (Arg2_is_const[line] ? Arg2[line]
                : IREG[Arg2[line]]);
        arg3 = (Arg3_is_const[line] ? Arg3[line]
                : IREG[Arg3[line]]);
        IREG[Arg1[line]] = arg2 * arg3;
        break;
      case IDIV:
        arg2 = (Arg2_is_const[line] ? Arg2[line]
                : IREG[Arg2[line]]);
        arg3 = (Arg3_is_const[line] ? Arg3[line]
                : IREG[Arg3[line]]);
        IREG[Arg1[line]] = arg2 / arg3;
        break;
      case ILOD:
        arg2 = (Arg2_is_const[line] ? Arg2[line]
                : IREG[Arg2[line]]);
        arg3 = (Arg3_is_const[line] ? Arg3[line]
                : IREG[Arg3[line]]);
        IREG[Arg1[line]] = IMEM[arg2 + arg3];
        break;
      case ISTR:
        arg2 = (Arg2_is_const[line] ? Arg2[line]
                : IREG[Arg2[line]]);
        arg3 = (Arg3_is_const[line] ? Arg3[line]
                : IREG[Arg3[line]]);
        IMEM[arg2 + arg3] = IREG[Arg1[line]];
        break;
      case RASN:
        if (Arg2_is_const[line])
          RREG[Arg1[line]] = Arg2_real_const[line];
        else
          RREG[Arg1[line]] = RREG[Arg2[line]];
        break;
      case RPRT:
        if (Arg1_is_const[line])
          printf ("%f ", Arg1_real_const[line]);
        else
          printf ("%f ", RREG[Arg1[line]]);
        break;
      case RINP:
        printf ("Input real?:");
        scanf ("%f", &arg2_f);
        RREG[Arg1[line]] = arg2_f;
        break;
      case REQL:
        arg2_f = (Arg2_is_const[line] ? Arg2_real_const[line]
                : RREG[Arg2[line]]);
        arg3_f = (Arg3_is_const[line] ? Arg3_real_const[line]
                : RREG[Arg3[line]]);
        IREG[Arg1[line]] = (arg2_f == arg3_f ? 1 : 0);
        break;
      case RNQL:
        arg2_f = (Arg2_is_const[line] ? Arg2_real_const[line]
                : RREG[Arg2[line]]);
        arg3_f = (Arg3_is_const[line] ? Arg3_real_const[line]
                : RREG[Arg3[line]]);
        IREG[Arg1[line]] = (arg2_f != arg3_f ? 1 : 0);
        break;
      case RLSS:
        arg2_f = (Arg2_is_const[line] ? Arg2_real_const[line]
                : RREG[Arg2[line]]);
        arg3_f = (Arg3_is_const[line] ? Arg3_real_const[line]
                : RREG[Arg3[line]]);
        IREG[Arg1[line]] = (arg2_f < arg3_f ? 1 : 0);
        break;
      case RGRT:
        arg2_f = (Arg2_is_const[line] ? Arg2_real_const[line]
                : RREG[Arg2[line]]);
        arg3_f = (Arg3_is_const[line] ? Arg3_real_const[line]
                : RREG[Arg3[line]]);
        IREG[Arg1[line]] = (arg2_f > arg3_f ? 1 : 0);
        break;
      case RADD:
        arg2_f = (Arg2_is_const[line] ? Arg2_real_const[line]
                : RREG[Arg2[line]]);
        arg3_f = (Arg3_is_const[line] ? Arg3_real_const[line]
                : RREG[Arg3[line]]);
        RREG[Arg1[line]] = arg2_f + arg3_f;
        break;
      case RSUB:
        arg2_f = (Arg2_is_const[line] ? Arg2_real_const[line]
                : RREG[Arg2[line]]);
        arg3_f = (Arg3_is_const[line] ? Arg3_real_const[line]
                : RREG[Arg3[line]]);
        RREG[Arg1[line]] = arg2_f - arg3_f;
        break;
      case RMLT:
        arg2_f = (Arg2_is_const[line] ? Arg2_real_const[line]
                : RREG[Arg2[line]]);
        arg3_f = (Arg3_is_const[line] ? Arg3_real_const[line]
                : RREG[Arg3[line]]);
        RREG[Arg1[line]] = arg2_f * arg3_f;
        break;
      case RDIV:
        arg2_f = (Arg2_is_const[line] ? Arg2_real_const[line]
                : RREG[Arg2[line]]);
        arg3_f = (Arg3_is_const[line] ? Arg3_real_const[line]
                : RREG[Arg3[line]]);
        RREG[Arg1[line]] = arg2_f / arg3_f;
        break;
      case RLOD:
        arg2 = (Arg2_is_const[line] ? Arg2[line]
                : IREG[Arg2[line]]);
        arg3 = (Arg3_is_const[line] ? Arg3[line]
                : IREG[Arg3[line]]);
        RREG[Arg1[line]] = RMEM[arg2 + arg3];
        break;
      case RSTR:
        arg2 = (Arg2_is_const[line] ? Arg2[line]
                : IREG[Arg2[line]]);
        arg3 = (Arg3_is_const[line] ? Arg3[line]
                : IREG[Arg3[line]]);
        RMEM[arg2 + arg3] = RREG[Arg1[line]];
        break;
      case ITOR:
        arg2 = (Arg2_is_const[line] ? Arg2[line]
                : IREG[Arg2[line]]);
        RREG[Arg1[line]] = arg2;
        break;
      case RTOI:
        arg3_f = (Arg2_is_const[line] ? Arg2_real_const[line]
                : RREG[Arg2[line]]);
        IREG[Arg1[line]] = arg3_f;
        break;
      case JUMP:
        next_line = Arg1[line];
        break;
      case JLINK:
	IREG[0] = next_line;
        next_line = Arg1[line];
        break;
      case RETRN:
        next_line = IREG[0];
        break;
      case JMPZ:
        arg1 = (Arg1_is_const[line] ? Arg1[line]
                : IREG[Arg1[line]]);
        if (arg1 == 0)
          next_line = Arg2[line];
        break;
      case JMPNZ:
        arg1 = (Arg1_is_const[line] ? Arg1[line]
                : IREG[Arg1[line]]);
        if (arg1 != 0)
          next_line = Arg2[line];
        break;
      case HALT:
	printf ("Reached Halt.\n");
        next_line = 0;
        break;
      default:
        printf ("REACHED DEFAULT. THIS IS A BUG.\n");
        break;
    }

  return next_line;
}

int run_program(int lines)
{
  int error = 0;
  int current_line = 1;

  while (current_line >= 1 && current_line <= lines)
    current_line = execute_line (current_line);

  error = 0;

  return error;
}

int main(int argc, char* argv[])
{
  int error;
  int lines = 0;

  if (argc != 2)
    printf ("Usage: %s <name of riski file>\n", argv[0]);
  else
    lines = load_program (argv[1]);

  if (lines > 0)
    error = run_program (lines);

  return error;
}

