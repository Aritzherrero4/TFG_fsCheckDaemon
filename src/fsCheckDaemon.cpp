#include "../include/fsCheckDaemon.hpp"
/* Signal_handler for the daemon*/
void signal_handler(int sig) {
    if (sig == SIGTERM) {
        fprintf (log_file,SD_INFO "SIGTERM Received, daemon exiting\n");
        fclose(log_file);
        delete tree;
        sd_bus_slot_unref(slot);
        sd_bus_flush_close_unref(bus);
        exit (1);
    }
    if (sig == SIGINT) {
        fprintf (log_file,SD_INFO "SIGINT Received, daemon exiting\n");
        fclose(log_file);
        delete tree;
        sd_bus_slot_unref(slot);
        sd_bus_flush_close_unref(bus);
        exit (1);
    }
    if (sig == SIGHUP){ //At this point, it will exit
        fprintf (log_file,SD_INFO "SIGHUP Received, reloading config\n");
        fclose(log_file);
        delete tree;
        sd_bus_slot_unref(slot);
        sd_bus_flush_close_unref(bus);
        exit (1);        
    }
}

/*The bus method of the daemon
 * When called from another program, it will extract the path and event id and call the apropiated 
 * function to update the tree */
static int method_getUpdate(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
    const char *str;
    int change;
    int r;

    r = sd_bus_message_read(m, "is",&change, &str);
    fs::path path = str;

    fprintf(log_file, SD_INFO "Received message: %s - %d\n", path.c_str(), change);
    fflush(log_file);
    switch(change){
        case 1:
            file_added(path);
            break;
        case 2:
            file_deleted(path);
            break;
        case 3:
            file_updated(path);
            break;
    }
    /**Integration**/
    std::thread t(send_bus_message, tree->root_hash);
    t.detach();
    
    fflush(log_file);
    
    /*Send the reply even if we expect the call to be set as "No-reply"
     *This fixes some possible issues with bad configured clients */
    sd_bus_reply_method_return(m, "i", 1);   
    return r;
}
/**Integration**/
int send_bus_message(std::string hash){
        int r=0;
        sd_bus_message *m = NULL;
                       
        r = sd_bus_message_new_method_call(bus, &m,
                                "net.aitorbelenguer.manager",             /* service to contact */
                                "/net/aitorbelenguer/manager",            /* object path */
                                "net.aitorbelenguer.manager",             /* interface name */
                                "updateHash");                           /* method name */
        if (r < 0) {
                fprintf(log_file, "Failed to create new method call: %d\n", r);
                return r;
        }
        sd_bus_message_append(m,"s",hash.c_str());
        if (r < 0) {
            fprintf(log_file, "Failed to append the arguments: %d\n", r);
            return r;
        }     
        //Si queremos confirmación, esto deberá cambiar
        r = sd_bus_message_set_expect_reply(m, 0);                      /*Set No-reply*/
        if (r < 0) {
            fprintf(log_file, "Failed to set no reply: %d\n", r);
            return r;
        }      
        //the destionation buss is set to NULL, so the msg bus will be used
        //The cookie is also NULL
        r = sd_bus_send(NULL, m, NULL);                                 /* Send the message*/

        if (r < 0) {
                fprintf(log_file, "Failed to send the message: %d\n", r);
                return r;
        }
        fprintf(log_file, SD_INFO "Update hash message sent to the network integrity service\n");
        fflush(log_file);
        sd_bus_message_unref(m);
        return r; 
}


int initialize(){
    /* Create the log file */
    int r;
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
    /* Assign what signals to handle in signal_handler */
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);
    signal(SIGINT, signal_handler); //This will be removed

    /** Bus configuration **/
    r = sd_bus_open_user(&bus);
    if (r < 0) {
        fprintf(log_file, "Failed to connect to system bus: %s\n", strerror(-r));
        return r;
    }
    //Add the object using the vtable defined in .h
    r = sd_bus_add_object_vtable(bus, &slot, 
                        "/net/aritzherrero/fsCheck",  /* object path */
                        "net.aritzherrero.fsCheck",   /* interface name */
                        fscheck_vtable,
                        NULL);
    if (r < 0) {
        fprintf(log_file, "Failed to issue method call: %s\n", strerror(-r));
        return r;
    } 

    //Request a well-known name so clients can find the service
    r = sd_bus_request_name(bus, "net.aritzherrero.fsCheck", 0);
    if (r < 0) {
        fprintf(log_file, "Failed to acquire service name: %s\n", strerror(-r));
        return r;
    }

    return 0;

}
/* Define some functions to update the tree depending on the event */

void file_updated(fs::path p){
    tree->nodeChanged(p, 1);
    fprintf(log_file,SD_WARNING "File changed\n");
    fprintf(log_file,SD_INFO "Root hash: %s\n", tree->root_hash.c_str());
}

void file_deleted(fs::path p){
    tree->nodeChanged(p, 2);
    fprintf(log_file,SD_CRIT "File deleted\n"); //SD_WARNING
    fprintf(log_file,SD_INFO "Root hash: %s\n", tree->root_hash.c_str());   
}

void file_added(fs::path p){
    tree->addNode(p);
    fprintf(log_file,SD_WARNING "New file created\n"); //SD_WARNING
    fprintf(log_file,SD_INFO "Root hash: %s\n", tree->root_hash.c_str());       
}


int main(){
    /*Prepare and configure all the logging and the daemon structure related
     *The dbus conenection is also estabilished */
    int r; 
    r = initialize();
    if (r < 0)
        exit(r);
    /*Read the path from the configuration file*/
    fs::path p;
    r = getPathFromConfig("/home/aritz/TFG-aritz/fsCheckDaemon/fsCheck.config", &p);
    if (r < 0){
        fprintf(log_file, "Failed to read configuration file.");
        exit(r);
    }
    
    //get the hash algorithm to use and initialize the merkle tree
    int m;
    r = getHashModeFromConfig("/home/aritz/TFG-aritz/fsCheckDaemon/fsCheck.config", &m);
    if (r == -1){
        fprintf(log_file, SD_ERR "Failed to read configuration file.\n");
        exit(r);   
    }
    else if (r == -2){
        fprintf(log_file, SD_WARNING "Incorrect hash mode specified. using Blake3...\n");  
    }
    fprintf (log_file, SD_INFO "Using %s%s", (m==0) ? ("BLAKE3") : ("SHA256"), " hash algotithm\n");
    tree = new Mtree(m);
    tree->populateTree(p);

    fprintf(log_file, SD_INFO "Config file read. Path:%s\n", p.c_str());
    fprintf(log_file,SD_INFO "Tree initialized\n");
    fprintf(log_file,SD_INFO "Root hash: %s\n", tree->root_hash.c_str());
    p.~path(); //Free the path string to prevent memory leaks if exit is called.

    //Send initial hash to the network
    fprintf(log_file, SD_INFO "Sending the initial root hash message\n");
    send_bus_message(tree->root_hash);
    fflush(log_file);
    //At this point, the daemon will wait until something new happens
    //Read the updates from the kernel module
    while(1){
        /* Process requests */
        r = sd_bus_process(bus, NULL);
        if (r < 0) {
                fprintf(log_file, "Failed to process bus: %s\n", strerror(-r));
                goto dbus_error;
        }
        if (r > 0) /* we processed a request, try to process another one, right-away */
                continue;
        /* Wait for the next request to process */
        r = sd_bus_wait(bus, (uint64_t) -1);
        if (r < 0) {
                fprintf(log_file, "Failed to wait on bus: %s\n", strerror(-r));
                goto dbus_error;
        }
    }
//If a DBUS related error is detected, jump here
//unreference from dbus and exit
dbus_error:
        fclose(log_file);
        delete tree;
        sd_bus_slot_unref(slot);
        sd_bus_flush_close_unref(bus);
        exit (r < 0 ? EXIT_FAILURE : EXIT_SUCCESS);
}

