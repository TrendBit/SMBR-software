#include "can_interface.hpp"

CAN::Interface::Interface(const std::string& interface){
    if (!Init_socket(interface)) {
        throw std::runtime_error("Failed to initialize CAN socket");
    }
}

CAN::Interface::~Interface(){
    if (socket_fd >= 0) {
        close(socket_fd);
    }
}

bool CAN::Interface::Init_socket(const std::string& interface){
    socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_fd < 0) {
        return false;
    }

    // Allow socket reuse
    int reuse = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        return false;
    }

    struct ifreq ifr;
    strcpy(ifr.ifr_name, interface.c_str());
    if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0) {
        return false;
    }

    struct sockaddr_can addr;
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(socket_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        return false;
    }

    return true;
}

Application_message CAN::Interface::Receiver_loop(){
    struct can_frame frame;

    while (true) {
        ssize_t nbytes = read(socket_fd, &frame, sizeof(struct can_frame));

        if (nbytes < 0) {
            continue;
        }

        // Create Application_message from CAN frame
        CAN::Message can_msg(&frame);

        // Ignore non extended frames
        if (can_msg.Extended() == false) {
            continue;
        }

        Application_message app_msg(can_msg);

        auto type = Process_message(app_msg);

        if (type.has_value()) {
            return app_msg;
        }
    }
}

bool CAN::Interface::Send_message(const Application_message& message) {
    struct can_frame frame = message.to_msg();

    // Wait in order to suppress CAN bus overload
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    ssize_t nbytes = write(socket_fd, &frame, sizeof(struct can_frame));
    return nbytes == sizeof(struct can_frame);
}

std::optional<Codes::Message_type> CAN::Interface::Process_message(Application_message &app_message){
    Codes::Module target_module = app_message.Module_type();
    if ((target_module != Codes::Module::All) and (target_module != Codes::Module::Any) and (target_module != module_type)) {
        std::cout << "Message for different module" << std::endl;
        return std::nullopt;
    }

    Codes::Instance target_instance = app_message.Instance_enumeration();
    if (target_instance != Codes::Instance::All and target_instance != instance) {
        std::cout << "Message for different instance" << std::endl;
        return std::nullopt;
    }

    auto message_type = app_message.Message_type();
    if(std::find(accepted_messages.begin(), accepted_messages.end(), message_type) == accepted_messages.end()){
        std::cout << "Message type not accepted" << std::endl;
        return std::nullopt;
    }

    return message_type;
}
