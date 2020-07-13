#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"
// LOOK
/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);

  FILE* fbranches = fopen(".beargit/.branches", "w");
  fprintf(fbranches, "%s\n", "master");
  fclose(fbranches);
   
  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
  write_string_to_file(".beargit/.current_branch", "master");

  return 0;
}


/* beargit add <filename>
 * 
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");
  char *dest = malloc(FILENAME_SIZE);
  strcat(dest, "./beargit/");
  char *src = malloc(FILENAME_SIZE);
  strcat(src, "./");

  char line[FILENAME_SIZE];

  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
  
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR: File %s already added\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  memset(dest, 0, FILENAME_SIZE);
  memset(src, 0, FILENAME_SIZE);
  strcat(dest, ".beargit/");
  strcat(dest, filename);
  strcat(src, "./");
  strcat(src, filename);

  fs_cp(src, dest);
  printf("%s %s\n", src, dest);

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}


/* beargit rm <filename>
 * 
 * See "Step 2" in the homework 1 spec.
 *
 */

int beargit_rm(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");
  int found;

  found = 0;

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      found = 1;
    } else {
      fprintf(fnewindex, "%s\n", line);
    }
  }

  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");


  if (!found) {
    fprintf(stderr, "ERROR: File %s not tracked", filename);
    return 1;
  }

  return 0;
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */

const char* go_bears = "GO BEARS!";

int is_commit_msg_ok_aux(const char* msg, int position) {
  int i;

  i = position;

  if (position > strlen(msg)) {
    return 0;
  }

  while(msg[i] == go_bears[i]) {
    i++;
  }

  if (--i == strlen(go_bears)) {
    return 1;
  } else {
    return is_commit_msg_ok_aux(msg, position + 1) ;
  }
  
}

int is_commit_msg_ok(const char* msg) {
  return is_commit_msg_ok_aux(msg, 0);
}

void lfsr_calculate(unsigned short *reg) {
  unsigned short bit = ((*reg >> 0) ^ (*reg >> 2) ^ (*reg >> 3) ^ (*reg >> 5)) & 1;
  *reg = (*reg >> 1) | (bit << 15); 
}

void next_commit_id_hw1(char* commit_id) {
  int i;
  char *s = "61c";
  unsigned short c = (unsigned short)commit_id[0];
  int count = COMMIT_ID_SIZE - COMMIT_ID_BRANCH_BYTES;
  
  lfsr_calculate(&c);
  
  for (i = 0; i < count; i++) {
    
    commit_id[i] = s[c % 3];
    
    lfsr_calculate(&c);
  }
  commit_id[count - 1] = '\0';
}

int beargit_commit_hw1(const char* msg) {
  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  int f = strlen(".beargit/") + COMMIT_ID_SIZE;
  char *folder = (char *)malloc(f + 1);
  folder[f] = 0;
  strcpy(folder, ".beargit/");

  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  
  next_commit_id(commit_id);


  /* COMPLETE THE REST */
  strcat(folder, commit_id);
  fs_mkdir(folder);
  
  
  
  FILE* findex = fopen(".beargit/.index", "r");
  char *dest = malloc(FILENAME_SIZE);
  char *src = malloc(FILENAME_SIZE);
  int size = strlen(folder);
  
  
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    memset(dest, 0,  FILENAME_SIZE);
    memset(src, 0,  FILENAME_SIZE);
    
    strcpy(dest, folder);
    strcat(dest, "/");
    strcat(dest, line);
    
    strcpy(src, ".beargit/");
    strcat(src, line);
    fs_mv(src, dest);
  }

  memset(dest, 0,  FILENAME_SIZE);
  memset(src, 0,  FILENAME_SIZE);

  strcpy(dest, folder);
  strcat(dest, "/.index");
  printf("copying to %s\n", dest);
  // copy index, prev
  fs_cp(".beargit/.index", dest);

  strcpy(dest, folder);
  strcat(dest, "/.prev");
  printf("copying to %s\n", dest);
  // copy index, prev
  fs_cp(".beargit/.prev", dest);

  write_string_to_file(".beargit/.prev", commit_id);


  free(dest);
  free(src);

  fclose(findex);
  free(folder);

  


  return 0;
}

/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */

int beargit_status() {
  FILE* findex = fopen(".beargit/.index", "r");
  int count;

  char line[FILENAME_SIZE];
  printf("%s", "Tracked files: \n");

  while(fgets(line, sizeof(line), findex)) {
    printf("%s", line);
    count++;
  }

  printf("%d%s", count, " files total\n");
  fclose(findex);
  return 0;
}

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 *
 */

int beargit_log() {
  /* COMPLETE THE REST */

  return 0;
}

// ---------------------------------------
// HOMEWORK 2 
// ---------------------------------------

// This adds a check to beargit_commit that ensures we are on the HEAD of a branch.
int beargit_commit(const char* msg) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  if (strlen(current_branch) == 0) {
    fprintf(stderr, "ERROR: Need to be on HEAD of a branch to commit\n");
    return 1;
  }

  return beargit_commit_hw1(msg);
}

const char* digits = "61c";

void next_commit_id(char* commit_id) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  // The first COMMIT_ID_BRANCH_BYTES=10 characters of the commit ID will
  // be used to encode the current branch number. This is necessary to avoid
  // duplicate IDs in different branches, as they can have the same pre-
  // decessor (so next_commit_id has to depend on something else).
  int n = get_branch_number(current_branch);
  
  for (int i = 0; i < COMMIT_ID_BRANCH_BYTES; i++) {
    // This translates the branch number into base 3 and substitutes 0 for
    // 6, 1 for 1 and c for 2.
    commit_id[i] = digits[n%3];
    n /= 3;
  }
  
  // Use next_commit_id to fill in the rest of the commit ID.
  next_commit_id_hw1(commit_id + COMMIT_ID_BRANCH_BYTES);
}


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char* branch_name) {
  FILE* fbranches = fopen(".beargit/.branches", "r");

  int branch_index = -1;
  int counter = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");
    if (strcmp(line, branch_name) == 0) {
      branch_index = counter;
    }
    counter++;
  }

  fclose(fbranches);

  return branch_index;
}

/* beargit branch
 *
 * See "Step 6" in the homework 1 spec.
 *
 */

int beargit_branch() {
  FILE* fbranches = fopen(".beargit/.branches", "r");
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  char line[FILENAME_SIZE];
  int index = 0;
  int branch_index = get_branch_number(current_branch);
  printf("%s - %d\n", current_branch, branch_index);
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");
    
    if (index == branch_index) {
      printf("*%s\n", line);
    } else {
      printf("%s\n", line);
    }
    index++;
  }

  fclose(fbranches);

  return 0;
}

/* beargit checkout
 *
 * See "Step 7" in the homework 1 spec.
 *
 */

int checkout_commit(const char* commit_id) {
  
//  Going through the index of the current index file, delete all those files 
//  (in the current directory; i.e., the directory where we ran beargit).

//  Copy the index from the commit that is being checked out to the .beargit directory,
//  and use it to copy all that commit's tracked files from the commit's directory into 
//  the current directory.

//  Write the ID of the commit that is being checked out into .prev.
//  In the special case that the new commit is the 00.0 commit, there are no files to copy and there is
//  no index. 
// Instead empty the index (but still write the ID into .prev and delete the current index files). 
// You may wonder how we could ever check out the 00.0 commit, since it is not a valid commit ID; the answer is that if you check out a branch whose HEAD is the 00.0 commit, that checkout is expected to work (while 00.0 would not be recognized as a commit ID).
  
  char line[FILENAME_SIZE];
  char filename[FILENAME_SIZE];
  char commitIndex[FILENAME_SIZE];
  char source[FILENAME_SIZE];
  char dest[FILENAME_SIZE];
  
  FILE* findex = fopen(".beargit/.index", "r");
  
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    memset(filename, 0, FILENAME_SIZE);
    strcpy(filename, ".beargit/");
    strcat(filename, line);
    printf("removing: %s/\n", filename);
    fs_rm(line);
  }

  
  if (strcmp(commit_id, "0000000000000000000000000000000") == 0) {
    fclose(fopen(".beargit/.index", "w"));
  } else {
    memset(commitIndex, 0, FILENAME_SIZE);
    strcpy(commitIndex, ".beargit/");
    strcat(commitIndex, commit_id);
    strcat(commitIndex, "/.index");  
    
    findex = fopen(commitIndex, "r");
    printf("%s %s\n", commitIndex, commit_id);

    while(fgets(line, sizeof(line), findex)) {
      strtok(line, "\n");
      memset(source, 0, FILENAME_SIZE);
      memset(dest, 0, FILENAME_SIZE);
      
      strcpy(source, ".beargit/");
      strcat(source, commit_id);
      strcat(source, "/");
      strcat(source, line);

      strcpy(dest, "./");
      strcat(dest, line);

      strcat(filename, line);
      printf("copying: from %s to %s/\n", source, dest);
      fs_cp(source, dest);
    }

    fclose(findex);
  }

  return 0;
}

int is_it_a_commit_id(const char* commit_id) {
  int i;
  char c;
  for (i = 0; i < COMMIT_ID_BYTES; i++) {
    c = commit_id[i];
    if (!(c == '6' || c == '1' || c == 'c'))  {
      return 0;
    }
  }
  
  return 1;
}

int beargit_checkout(const char* arg, int new_branch) {
  // Get the current branch
  char current_branch[BRANCHNAME_SIZE];
  
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
  
  // If not detached, update the current branch by storing the current HEAD into that branch's file...
  // Even if we cancel later, this is still ok.
  if (strlen(current_branch)) {
    char current_branch_file[BRANCHNAME_SIZE+50];
    sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
    fs_cp(".beargit/.prev", current_branch_file);
  }
  
  // Check whether the argument is a commit ID. If yes, we just stay in detached mode
  // without actually having to change into any other branch.
  if (is_it_a_commit_id(arg)) {
    char commit_dir[FILENAME_SIZE];
    strcat(commit_dir, ".beargit/");
    strcat(commit_dir, arg);
    if (!fs_check_dir_exists(commit_dir)) {
      fprintf(stderr, "ERROR: Commit %s does not exist\n", arg);
      return 1;
    }
    
    
    // // Set the current branch to none (i.e., detached).
    write_string_to_file(".beargit/.current_branch", "");

    return checkout_commit(arg);
  }

  
  

  // Just a better name, since we now know the argument is a branch name.
  const char* branch_name = arg;
  
  // Read branches file (giving us the HEAD commit id for that branch).
  int branch_exists = (get_branch_number(branch_name) >= 0);
  
  // Check for errors.
  if (branch_exists) {
    fprintf(stderr, "ERROR: A branch named %s already exists\n", branch_name);
    return 1;
  } else if (!branch_exists && !new_branch) {
    fprintf(stderr, "ERROR: No branch %s exists\n", branch_name);
    return 1;
  }

  // File for the branch we are changing into.
  char branch_file[FILENAME_SIZE]; 
  strcpy(branch_file, ".beargit/.branch_");
  strcat(branch_file, branch_name);
  branch_file[17 + strlen(branch_name)] = '\0';

  

  // Update the branch file if new branch is created (now it can't go wrong anymore)
  if (new_branch) {
    FILE* fbranches = fopen(".beargit/.branches", "a");
    fprintf(fbranches, "%s\n", branch_name);
    fclose(fbranches);
    printf("%s\n", branch_file);
    fs_cp(".beargit/.prev", branch_file); 
  }
  
  
  write_string_to_file(".beargit/.current_branch", branch_name);
  
  // Read the head commit ID of this branch.
  char branch_head_commit_id[COMMIT_ID_SIZE];
  read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);
  
  // Check out the actual commit.
  return checkout_commit(branch_head_commit_id);
}
