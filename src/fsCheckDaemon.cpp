#include "../include/fsCheckDaemon.hpp"

void signal_handler(int sig) {
    if (sig == SIGTERM) {
        fprintf (log_file,SD_INFO "SIGTERM Received, daemon exiting\n");
        fclose(log_file);
        exit (1);
    }
    if (sig == SIGINT) {
      fprintf (log_file,SD_INFO "SIGINT Received, daemon exiting\n");
      fclose(log_file);
      exit (1);
    }
    if (sig == SIGHUP){
        fprintf (log_file,SD_INFO "SIGHUP Received, reloading config\n");
        exit (1);        
    }
}


void initialize(){
    int fd = sd_journal_stream_fd("fsCheckDaemon", LOG_INFO, 1);

    if (fd < 0) {
        fprintf(stderr, "Failed to create stream fd: %s\n", strerror(-fd));
        exit(1);
    }

    log_file = fdopen(fd, "w");

    if (!log_file) {
        fprintf(stderr, "Failed to create file object: %m\n");
        close(fd);
        exit(1);
    }

    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);
    signal(SIGINT, signal_handler); //This will be removed

}

void file_updated(fs::path p, Mtree * tree){
    tree->nodeChanged(p, 1);
    fprintf(log_file,SD_WARNING "File changed\n");
    fprintf(log_file,SD_INFO "Root hash: %s\n", tree->root_hash.c_str());
}

void file_deleted(fs::path p, Mtree * tree){
    tree->nodeChanged(p, 2);
    fprintf(log_file,SD_CRIT "File deleted\n"); //SD_WARNING
    fprintf(log_file,SD_INFO "Root hash: %s\n", tree->root_hash.c_str());   
}

void file_added(fs::path p, Mtree *tree){
    tree->addNode(p);
    fprintf(log_file,SD_WARNING "New file created\n"); //SD_WARNING
    fprintf(log_file,SD_INFO "Root hash: %s\n", tree->root_hash.c_str());       
}


int main(){

    //Prepare and configure all the logging and the daemon structure related
    initialize();

    /* In the future, the path to control will be read from a configuration file 
    *  This file will be the same for both the daemon and the module 
    *  For now, it will be hardcoded here
    */
    fs::path p = "/home/aritz/test/root/";

    //Initialize the merkle tree
    Mtree * tree = new Mtree();
    tree->populateTree(p);

    fprintf(log_file,SD_INFO "Tree initialized\n");
    fprintf(log_file,SD_INFO "Root hash: %s\n", tree->root_hash.c_str());
    fflush(log_file);

    //At this point, the daemon will wait until something new happens
    //Read the updates from the kernel module 
    while(1){
        sleep(10);

    }

}