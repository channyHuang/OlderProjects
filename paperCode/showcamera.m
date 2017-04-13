%show dtam data
function showcamera()
    [R1 t1] = readSfm();
    [R2 t2] = readDtam();
    if 1
        for i = 1:size(R1, 3)
            errR(i) = norm(rodrigues(R1(:,:,i)) - rodrigues(R2(:,:,i)));
            errt(i) = norm(t1(:,:,i)./sum(t1(:,:,i)) - t2(:,:,i)./sum(t2(:,:,i)));
            %errt(i) = norm(t1(:,:,i) - t2(:,:,i));
        end
        meanRt = [mean(errR), mean(errt)];
        stdRt = [std(errR), std(errt)];
        figure, bar(errR);
        line([0 120], [meanRt(1) meanRt(1)]);
        title('error bar of Rotation matrix');
        legend(['\mu = ' num2str(meanRt(1)) ',\sigma = ' num2str(stdRt(1))]);
        xlabel('frame');
        ylabel('error of R');
        figure, bar(errt);
        legend(['\mu = ' num2str(meanRt(2)) ',\sigma = ' num2str(stdRt(2))]);
        line([0 120], [meanRt(2) meanRt(2)]);
        title('error bar of Translation matrix');
        xlabel('frame');
        ylabel('error of t');
    end
    
    if 0
        path = 'E:/RealFootData/data_2/sfmCalrt';
        for i = 1:size(R1, 3)
            filename = [path '/' num2str(i-1, '%08d') '.txt'];
            fin = fopen(filename, 'wt');
            for x = 1:3
                for y = 1:3
                    fprintf(fin, '%f ', R1(x, y, i));
                end
                fprintf(fin, '\n');
            end
            for x = 1:3
                fprintf(fin, '%f ', t1(x, 1, i));
            end
            fprintf(fin, '\n');
            fclose(fin);
        end
    end
end

function [R, t] = readSfm()     
if 1    
    filenum = 100;
    f = 1400;
    K = [f 0 960; 0, f, 540; 0, 0, 1];
    path = 'E:/RealFootData/data_2/sfmResult/visualSFM.nvm.cmvs/00/cameras_v2.txt';
    fin = fopen(path);
    index = zeros(filenum, 1);
    sfmIndex = 1;
    while 1
        line = fgetl(fin);
        if (length(line) > 0 && (strcmp(line(1:2), '00') == 1) )
            line = fgetl(fin);
            orgIndex = sscanf(line, 'E:/RealFootData/data_2/pic/%d.jpg');
            index(sfmIndex) = orgIndex + 1;
            sfmIndex = sfmIndex + 1;
            if (sfmIndex > filenum)
                break;
            end
        end
    end
    fclose(fin);
    
    path = 'E:/RealFootData/data_2/sfmResult/visualSFM.nvm.cmvs/00/txt';
    txtfiles = dir([path '/*.txt']);
    filenum = length(txtfiles);
    P = zeros(3, 4, filenum);
    tmpR = zeros(3, 3, filenum);
    R = zeros(3, 3, filenum);
    tmpt = zeros(3, 1, filenum);
    t = zeros(3, 1, filenum);
    for i = 1:filenum
       txtfile = [path '/' txtfiles(i).name];
       fin = fopen(txtfile);
       line = fgetl(fin);
       for j = 1:3
           line = fgetl(fin);
           r = sscanf(line, '%f %f %f');
           P(j,:,index(i)) = r;
       end
       RT = inv(K)*P(:, :, index(i));
       R(:, :,index(i)) = RT(1:3, 1:3);
       t(:,:,index(i)) = RT(1:3, 4);
       fclose(fin);
    end
end

    

   figure, draw(R, t);
   title('Coordinate of sfm result');
   xlabel('x');
    ylabel('y');
    zlabel('z');
end

function [R, t] = readDtam() 
    path = 'E:/RealFootData/data_2/calrt';
   txtfiles = dir([path '/*.txt']); 
   filenum = length(txtfiles);
   R = zeros(3, 3, filenum);
   t = zeros(3, 1, filenum);
   for i = 1:filenum
       txtfile = [path '/' txtfiles(i).name];
       fin = fopen(txtfile);      
       for j = 1:3
           line = fgetl(fin);
           R(j,:,i) = sscanf(line, '%f %f %f');
       end
       line = fgetl(fin);
       t(:,:,i) = sscanf(line, '%f %f %f');
       fclose(fin);
       
   end
   
    figure, draw(R, t);
    title('Coordinate of our result');
    xlabel('x');
    ylabel('y');
    zlabel('z');
end

function draw(R, t)
    axis = [0, 0, 0; 1, 0, 0; 0, 1, 0; 0, 0, 1];
%    drawsingle(axis, 0);
    for i = 1:size(R, 3)
        naxis = (axis + [t(:, :, i)';t(:, :, i)';t(:, :, i)';t(:, :, i)'])*R(:, :, i);
        drawsingle(naxis, i);
    end
end

function drawsingle(axis, no)
    hold on;
    line([axis(1, 1), axis(2, 1)], [axis(1, 2), axis(2, 2)],[axis(1, 3), axis(2, 3)], 'Color', 'r');
    view(3);
    line([axis(1, 1), axis(3, 1)], [axis(1, 2), axis(3, 2)],[axis(1, 3), axis(3, 3)], 'Color', 'g');
    line([axis(1, 1), axis(4, 1)], [axis(1, 2), axis(4, 2)],[axis(1, 3), axis(4, 3)], 'Color', 'b');
    
    text(axis(1, 1), axis(1, 2), axis(1, 3), num2str(no));
end